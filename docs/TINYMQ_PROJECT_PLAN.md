# TinyMQ Project Plan

## 프로젝트 개요

**목표**: Kafka 스타일의 단일 노드 메시지 큐 시스템 구현
**예상 기간**: 8-9주 (주 5시간, 총 40-45시간)
**난이도**: 중상급

---

## 1. 아키텍처 개요

```
┌─────────────────────────────────────────────────────────────────┐
│                         TinyMQ Server                           │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │
│  │  Producer   │  │  Producer   │  │  Consumer   │   Clients    │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘              │
│         │                │                │                      │
├─────────┴────────────────┴────────────────┴─────────────────────┤
│                      TCP Connection Layer                        │
│                    (epoll + non-blocking I/O)                    │
├─────────────────────────────────────────────────────────────────┤
│                      Protocol Parser Layer                       │
│              (Command parsing, Request/Response)                 │
├─────────────────────────────────────────────────────────────────┤
│                       Broker Core Layer                          │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                    Topic Manager                         │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐      │    │
│  │  │ Topic: logs │  │Topic: events│  │Topic: metrics│     │    │
│  │  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘      │    │
│  └─────────┴────────────────┴────────────────┴──────────────┘    │
├─────────────────────────────────────────────────────────────────┤
│                      Storage Engine Layer                        │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │                   Segment Manager                        │    │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐               │    │
│  │  │SegmentLog│  │SegmentIdx│  │ Offset   │               │    │
│  │  │ (.log)   │  │ (.idx)   │  │ Manager  │               │    │
│  │  └──────────┘  └──────────┘  └──────────┘               │    │
│  └─────────────────────────────────────────────────────────┘    │
├─────────────────────────────────────────────────────────────────┤
│                         File System                              │
│                    (Append-only Logs + Index)                    │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. 디렉토리 구조 (최종)

```
tiny-mon/
├── CMakeLists.txt
├── README.md
├── docs/
│   └── TINYMQ_PROJECT_PLAN.md    # 이 문서
│
├── libsrc/
│   ├── libLogger.c               # [기존] 로깅 라이브러리
│   └── libLogger.h
│
├── src/
│   ├── main.cpp                  # 서버 진입점
│   │
│   ├── network/                  # 네트워크 레이어
│   │   ├── tcp_server.h          # epoll 기반 TCP 서버
│   │   ├── tcp_server.c
│   │   ├── connection.h          # 클라이언트 연결 관리
│   │   └── connection.c
│   │
│   ├── protocol/                 # 프로토콜 레이어
│   │   ├── command.h             # 명령어 정의
│   │   ├── parser.h              # 프로토콜 파서
│   │   ├── parser.c
│   │   ├── request.h             # 요청 구조체
│   │   └── response.h            # 응답 구조체
│   │
│   ├── broker/                   # 브로커 코어
│   │   ├── broker.h              # 메인 브로커 로직
│   │   ├── broker.c
│   │   ├── topic.h               # 토픽 관리
│   │   ├── topic.c
│   │   ├── consumer_group.h      # 컨슈머 그룹
│   │   └── consumer_group.c
│   │
│   ├── storage/                  # 스토리지 엔진
│   │   ├── segment.h             # 세그먼트 파일
│   │   ├── segment.c
│   │   ├── index.h               # 인덱스 파일
│   │   ├── index.c
│   │   ├── message.h             # 메시지 포맷
│   │   └── retention.c           # 보존 정책
│   │
│   └── utils/                    # 유틸리티
│       ├── buffer.h              # 버퍼 관리
│       ├── buffer.c
│       ├── crc32.h               # 체크섬
│       └── crc32.c
│
├── client/                       # 테스트용 클라이언트
│   ├── producer.c                # Producer CLI
│   └── consumer.c                # Consumer CLI
│
├── data/                         # 런타임 데이터 (gitignore)
│   └── topics/
│       └── <topic_name>/
│           ├── 00000000000000000000.log
│           ├── 00000000000000000000.idx
│           └── consumer_offsets/
│               └── <group_id>.offset
│
└── tests/                        # 테스트
    ├── test_segment.c
    ├── test_parser.c
    └── test_broker.c
```

---

## 3. 프로토콜 명세

### 3.1 기본 형식

```
<COMMAND> <args...>\r\n
[payload if needed]
```

### 3.2 명령어 목록

#### Producer 명령어

```
# 토픽에 메시지 발행
PUBLISH <topic> <payload_length>\r\n
<payload_bytes>

# 응답
+OK <offset>\r\n
-ERR <error_message>\r\n
```

#### Consumer 명령어

```
# 토픽 구독 (Consumer Group 등록)
SUBSCRIBE <topic> <group_id>\r\n

# 응답
+OK\r\n
-ERR <error_message>\r\n
```

```
# 메시지 폴링
POLL <topic> <group_id> [max_messages] [timeout_ms]\r\n

# 응답 (메시지가 있을 때)
+MESSAGES <count>\r\n
<offset1> <length1>\r\n
<payload1>
<offset2> <length2>\r\n
<payload2>
...

# 응답 (메시지가 없을 때)
+EMPTY\r\n
```

```
# Offset 커밋
COMMIT <topic> <group_id> <offset>\r\n

# 응답
+OK\r\n
-ERR <error_message>\r\n
```

#### 관리 명령어

```
# 토픽 생성
CREATE <topic>\r\n

# 토픽 삭제
DELETE <topic>\r\n

# 토픽 목록
TOPICS\r\n

# 토픽 정보 (offset 범위, 크기 등)
INFO <topic>\r\n

# 서버 상태
STATS\r\n

# 연결 종료
QUIT\r\n
```

### 3.3 에러 코드

| 코드 | 의미 |
|------|------|
| `ERR_UNKNOWN_CMD` | 알 수 없는 명령어 |
| `ERR_TOPIC_NOT_FOUND` | 토픽 없음 |
| `ERR_TOPIC_EXISTS` | 토픽 이미 존재 |
| `ERR_INVALID_OFFSET` | 잘못된 offset |
| `ERR_STORAGE_FULL` | 저장 공간 부족 |
| `ERR_INVALID_FORMAT` | 프로토콜 형식 오류 |

---

## 4. 데이터 구조

### 4.1 메시지 포맷 (디스크 저장)

```c
// 메시지 헤더 (고정 24바이트)
typedef struct __attribute__((packed)) {
    uint64_t offset;        // 8바이트: 메시지 순번 (0부터 시작)
    uint32_t payload_size;  // 4바이트: 페이로드 크기
    uint32_t crc32;         // 4바이트: 페이로드 체크섬
    int64_t  timestamp;     // 8바이트: Unix timestamp (ms)
} message_header_t;

// 전체 메시지 = header + payload
// [header 24bytes][payload N bytes]
```

### 4.2 인덱스 엔트리 (디스크 저장)

```c
// 인덱스 엔트리 (고정 16바이트)
typedef struct __attribute__((packed)) {
    uint64_t offset;        // 8바이트: 메시지 offset
    uint64_t position;      // 8바이트: .log 파일 내 위치
} index_entry_t;

// 인덱스는 sparse index (매 N개 메시지마다 기록)
#define INDEX_INTERVAL 100  // 100개 메시지마다 인덱스
```

### 4.3 세그먼트 구조

```c
typedef struct {
    char     base_name[256];    // 세그먼트 기본 이름 (offset 기반)
    int      log_fd;            // .log 파일 FD
    int      idx_fd;            // .idx 파일 FD
    uint64_t base_offset;       // 이 세그먼트의 시작 offset
    uint64_t next_offset;       // 다음 메시지가 받을 offset
    size_t   current_size;      // 현재 .log 파일 크기
    void*    idx_mmap;          // 인덱스 mmap 주소
    size_t   idx_mmap_size;     // mmap 크기
} segment_t;

#define SEGMENT_MAX_SIZE (100 * 1024 * 1024)  // 100MB per segment
```

### 4.4 토픽 구조

```c
typedef struct {
    char            name[64];           // 토픽 이름
    char            path[256];          // 토픽 디렉토리 경로
    segment_t*      active_segment;     // 현재 쓰기 중인 세그먼트
    segment_t**     segments;           // 모든 세그먼트 목록
    int             segment_count;      // 세그먼트 개수
    uint64_t        earliest_offset;    // 가장 오래된 offset
    uint64_t        latest_offset;      // 가장 최신 offset
    pthread_mutex_t lock;               // 토픽 레벨 락
} topic_t;
```

### 4.5 Consumer Group

```c
typedef struct {
    char     group_id[64];      // 그룹 ID
    char     topic[64];         // 구독 토픽
    uint64_t committed_offset;  // 커밋된 offset
    int      offset_fd;         // offset 파일 FD
} consumer_group_t;
```

### 4.6 클라이언트 연결

```c
typedef enum {
    CLIENT_TYPE_UNKNOWN,
    CLIENT_TYPE_PRODUCER,
    CLIENT_TYPE_CONSUMER
} client_type_t;

typedef struct {
    int             fd;                 // 소켓 FD
    client_type_t   type;               // 클라이언트 타입
    char            group_id[64];       // Consumer인 경우 그룹 ID
    char            recv_buffer[4096];  // 수신 버퍼
    size_t          recv_len;           // 수신된 데이터 길이
    char            send_buffer[4096];  // 송신 버퍼
    size_t          send_len;           // 전송 대기 데이터 길이
    size_t          send_offset;        // 전송된 위치
} client_t;
```

---

## 5. 주차별 구현 계획

### Week 1: 프로젝트 셋업 + 프로토콜 설계 (5시간)

**목표**: 기반 구조 확립

| 작업 | 시간 | 설명 |
|------|------|------|
| 디렉토리 구조 생성 | 0.5h | src/network, src/protocol 등 |
| CMakeLists.txt 수정 | 0.5h | 새 소스 파일 추가 |
| command.h 작성 | 1h | 명령어 enum, 상수 정의 |
| message.h 작성 | 1h | 메시지 헤더 구조체 |
| crc32 구현 | 1h | 체크섬 유틸리티 |
| 기본 테스트 | 1h | 빌드 확인 |

**학습 포인트**:
- `__attribute__((packed))` - 구조체 패딩 제거
- 바이트 오더 (리틀/빅 엔디안)

**산출물**:
- `src/protocol/command.h`
- `src/storage/message.h`
- `src/utils/crc32.c`

---

### Week 2: epoll 서버 완성 (5시간)

**목표**: 안정적인 비동기 TCP 서버

| 작업 | 시간 | 설명 |
|------|------|------|
| 기존 socket_server.c 버그 수정 | 1h | 오타, 로직 오류 |
| tcp_server.c 재작성 | 2h | 깔끔한 epoll 루프 |
| connection.c 구현 | 1.5h | 클라이언트 관리 |
| 에코 테스트 | 0.5h | nc로 연결 테스트 |

**학습 포인트**:
```c
epoll_create1(EPOLL_CLOEXEC)   // close-on-exec 플래그
epoll_ctl(EPOLL_CTL_ADD/MOD/DEL)
epoll_wait()
fcntl(fd, F_SETFL, O_NONBLOCK) // 논블로킹 설정
```

**산출물**:
- `src/network/tcp_server.c`
- `src/network/connection.c`

---

### Week 3: 프로토콜 파서 (5시간)

**목표**: 명령어 파싱 및 응답 생성

| 작업 | 시간 | 설명 |
|------|------|------|
| parser.c 구현 | 2h | 명령어 파싱 로직 |
| request.h 구현 | 1h | 파싱된 요청 구조체 |
| response.c 구현 | 1h | 응답 포맷팅 |
| 파서 테스트 | 1h | 단위 테스트 |

**학습 포인트**:
- 상태 기계 (State Machine) 패턴
- 부분 읽기 처리 (버퍼링)
- `strncmp`, `strtok_r` 안전한 문자열 처리

**산출물**:
- `src/protocol/parser.c`
- `src/protocol/request.h`
- `src/protocol/response.c`

---

### Week 4: 토픽 + 메모리 큐 (5시간)

**목표**: 인메모리 메시지 저장/조회

| 작업 | 시간 | 설명 |
|------|------|------|
| topic.c 구현 | 2h | 토픽 CRUD |
| broker.c 기본 구현 | 2h | PUBLISH/POLL 처리 |
| 메모리 큐 (ring buffer) | 1h | 임시 저장소 |

**학습 포인트**:
- `pthread_mutex_lock/unlock` - 동시성 제어
- Ring buffer 자료구조
- 해시 테이블 (토픽 검색)

**산출물**:
- `src/broker/topic.c`
- `src/broker/broker.c`
- 동작하는 인메모리 MQ!

**마일스톤 1**: 메모리 기반 메시지 큐 동작 ✅

---

### Week 5: 세그먼트 저장 (5시간)

**목표**: 메시지를 디스크에 영속화

| 작업 | 시간 | 설명 |
|------|------|------|
| segment.c - 쓰기 | 2h | append 로직 |
| segment.c - 읽기 | 2h | offset으로 검색 |
| 세그먼트 롤오버 | 1h | 크기 초과 시 새 파일 |

**학습 포인트**:
```c
open(path, O_WRONLY | O_APPEND | O_CREAT, 0644)
write(fd, buf, len)           // 메시지 쓰기
pread(fd, buf, len, offset)   // 위치 지정 읽기
fsync(fd)                     // 디스크 동기화
ftruncate(fd, size)           // 파일 크기 조정
```

**산출물**:
- `src/storage/segment.c`
- `data/topics/<name>/*.log` 파일 생성

---

### Week 6: 인덱스 + 빠른 검색 (5시간)

**목표**: offset으로 빠르게 메시지 찾기

| 작업 | 시간 | 설명 |
|------|------|------|
| index.c 구현 | 2h | sparse index 쓰기 |
| mmap 인덱스 읽기 | 2h | 메모리 매핑 |
| 이진 탐색 구현 | 1h | offset → position |

**학습 포인트**:
```c
mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0)
munmap(addr, size)
madvise(addr, size, MADV_SEQUENTIAL)  // 읽기 패턴 힌트
msync(addr, size, MS_SYNC)            // 동기화
```

**산출물**:
- `src/storage/index.c`
- `*.idx` 파일 생성 및 활용

---

### Week 7: Consumer Group + Offset (5시간)

**목표**: 여러 Consumer가 독립적으로 소비

| 작업 | 시간 | 설명 |
|------|------|------|
| consumer_group.c | 2h | 그룹 관리 |
| offset 저장/복구 | 2h | 커밋된 위치 기억 |
| SUBSCRIBE/COMMIT 연결 | 1h | 프로토콜 통합 |

**학습 포인트**:
- Consumer Group 개념
- At-least-once 전달 보장
- Offset 커밋 전략

**산출물**:
- `src/broker/consumer_group.c`
- `consumer_offsets/<group>.offset` 파일

**마일스톤 2**: 영속화 + Consumer Group 동작 ✅

---

### Week 8: Retention + 정리 (5시간)

**목표**: 오래된 데이터 자동 삭제

| 작업 | 시간 | 설명 |
|------|------|------|
| retention.c | 2h | 시간/크기 기반 삭제 |
| 백그라운드 정리 스레드 | 1.5h | 주기적 실행 |
| graceful shutdown | 1h | SIGINT/SIGTERM 처리 |
| 통합 테스트 | 0.5h | 전체 흐름 검증 |

**학습 포인트**:
```c
unlink(path)                  // 파일 삭제
rename(old, new)              // 원자적 교체
opendir/readdir/closedir      // 디렉토리 탐색
stat(path, &st)               // 파일 정보
signalfd(sigset)              // 시그널 FD
```

**산출물**:
- `src/storage/retention.c`
- 안정적인 종료 처리

---

### Week 9 (버퍼): 테스트 + 클라이언트 (5시간)

**목표**: 마무리 및 사용성 개선

| 작업 | 시간 | 설명 |
|------|------|------|
| producer.c CLI | 1.5h | 명령줄 Producer |
| consumer.c CLI | 1.5h | 명령줄 Consumer |
| 벤치마크 | 1h | 처리량 측정 |
| 문서화 | 1h | README 업데이트 |

**산출물**:
- `client/producer.c`
- `client/consumer.c`
- 성능 수치 기록

---

## 6. 시스템 콜 학습 맵

### 네트워크 I/O
```c
socket()        → Week 2
bind()          → Week 2
listen()        → Week 2
accept4()       → Week 2 (SOCK_NONBLOCK 플래그)
epoll_create1() → Week 2
epoll_ctl()     → Week 2
epoll_wait()    → Week 2
recv()          → Week 2
send()          → Week 2
```

### 파일 I/O
```c
open()          → Week 5
close()         → Week 5
write()         → Week 5
pread()         → Week 5
fsync()         → Week 5
ftruncate()     → Week 5
lseek()         → Week 6
```

### 메모리 매핑
```c
mmap()          → Week 6
munmap()        → Week 6
madvise()       → Week 6
msync()         → Week 6
```

### 파일시스템
```c
mkdir()         → Week 4
unlink()        → Week 8
rename()        → Week 8
opendir()       → Week 8
readdir()       → Week 8
stat()          → Week 8
```

### 프로세스/시그널
```c
pthread_create()      → Week 8
pthread_mutex_*()     → Week 4
signalfd()            → Week 8
```

---

## 7. 테스트 시나리오

### 기본 동작 테스트
```bash
# 터미널 1: 서버 시작
./bin/tinymq --port 9999

# 터미널 2: Producer
echo -e "CREATE logs\r" | nc localhost 9999
echo -e "PUBLISH logs 5\r\nhello" | nc localhost 9999
echo -e "PUBLISH logs 5\r\nworld" | nc localhost 9999

# 터미널 3: Consumer
echo -e "SUBSCRIBE logs group1\r" | nc localhost 9999
echo -e "POLL logs group1 10\r" | nc localhost 9999
echo -e "COMMIT logs group1 1\r" | nc localhost 9999
```

### 재시작 테스트
```bash
# 1. 메시지 100개 발행
# 2. 서버 재시작
# 3. Consumer가 기존 offset부터 소비하는지 확인
```

### 부하 테스트
```bash
# 초당 1000개 메시지 발행
# Consumer 처리량 측정
```

---

## 8. 성능 목표

| 지표 | 목표 |
|------|------|
| Producer 처리량 | > 10,000 msg/sec |
| Consumer 처리량 | > 20,000 msg/sec |
| 지연 시간 (P99) | < 10ms |
| 메시지 크기 | 1KB 기준 |

---

## 9. 확장 가능성 (향후)

프로젝트 완료 후 추가할 수 있는 기능:

| 기능 | 예상 시간 | 설명 |
|------|----------|------|
| Partition | +15h | 병렬 처리 |
| Batch 처리 | +5h | 여러 메시지 한 번에 |
| 압축 | +5h | LZ4/Snappy |
| TLS | +10h | 암호화 통신 |
| 웹 대시보드 | +15h | 모니터링 UI |
| 복제 | +30h | 고가용성 |

---

## 10. 참고 자료

### Kafka 내부 구조
- [Kafka: The Definitive Guide](https://www.confluent.io/resources/kafka-the-definitive-guide/)
- [Kafka Log Internals](https://kafka.apache.org/documentation/#log)

### Linux 시스템 프로그래밍
- The Linux Programming Interface (Michael Kerrisk)
- Linux System Programming (Robert Love)

### 관련 프로젝트
- [Redpanda](https://github.com/redpanda-data/redpanda) - Kafka 호환 C++
- [NATS](https://github.com/nats-io/nats-server) - Go 메시지 큐

---

## 11. 체크리스트

### Week 1
- [ ] 디렉토리 구조 생성
- [ ] CMakeLists.txt 업데이트
- [ ] command.h 완성
- [ ] message.h 완성
- [ ] crc32 구현 및 테스트

### Week 2
- [ ] socket_server.c 버그 수정
- [ ] tcp_server.c 구현
- [ ] connection.c 구현
- [ ] 에코 서버 동작 확인

### Week 3
- [ ] parser.c 구현
- [ ] request.h 정의
- [ ] response.c 구현
- [ ] 명령어 파싱 테스트

### Week 4
- [ ] topic.c 구현
- [ ] broker.c 기본 구현
- [ ] 메모리 큐 동작 확인
- [ ] **마일스톤 1 달성**

### Week 5
- [ ] segment.c 쓰기 구현
- [ ] segment.c 읽기 구현
- [ ] 세그먼트 롤오버 동작

### Week 6
- [ ] index.c 구현
- [ ] mmap 기반 읽기
- [ ] 이진 탐색 검증

### Week 7
- [ ] consumer_group.c 구현
- [ ] offset 저장/복구
- [ ] **마일스톤 2 달성**

### Week 8
- [ ] retention.c 구현
- [ ] 정리 스레드
- [ ] graceful shutdown

### Week 9
- [ ] CLI 클라이언트
- [ ] 벤치마크
- [ ] 문서화 완료

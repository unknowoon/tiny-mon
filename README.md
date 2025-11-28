# Petiq

Kafka 스타일 경량 메시지 큐 시스템 (petit + topique + queue)

> 프랑스어로 "작은(petit) 주제(topique)"와 영어 "큐(queue)"의 조합

## 프로젝트 목표

Apache Kafka의 핵심 개념을 학습하고, 이를 C/C++로 직접 구현하며 메시지 큐 시스템의 내부 동작을 이해하는 것을 목표로 합니다.

### 목표 기능 (로드맵)

- [ ] Topic 기반 메시지 발행/구독
- [ ] Partition을 통한 메시지 분산
- [ ] Consumer Group 지원
- [ ] 메시지 영속성 (로그 기반 저장)
- [ ] Producer/Consumer API

## 현재 상태

현재는 TCP 서버 기반의 기초 인프라가 구현되어 있습니다.

### 구현 완료

- TCP 소켓 서버 (epoll 기반)
- 클라이언트 연결 관리
- 로깅 시스템
- 메모리 DB 기초 구조

## 프로젝트 구조

```
petiq/
├── CMakeLists.txt          # CMake 빌드 설정
├── Dockerfile              # Docker 빌드 설정
├── shl/                    # 셸 스크립트
│   ├── build.sh            # 빌드 스크립트
│   └── clean.sh            # 클린 스크립트
├── bin/                    # 빌드된 실행 파일
├── libsrc/                 # 라이브러리 소스
│   ├── libLogger.h         # 로거 헤더
│   └── libLogger.c         # 로거 구현
└── src/                    # 애플리케이션 소스
    ├── main.cpp            # 메인 엔트리
    ├── socket_server.h/c   # 소켓 서버 (메인 서버 로직)
    ├── tcp_socket.h/c      # TCP 소켓 생성/설정
    ├── epoll_handler.h/c   # epoll 이벤트 처리
    ├── client_manager.h/c  # 클라이언트 연결 관리
    ├── comm.h/c            # 통신 모듈
    ├── commManager.h/c     # 연결 관리
    ├── mdb.hpp             # 메모리 DB
    └── example_echo.c      # 에코 서버 예제
```

## 빠른 시작

### 요구사항

- CMake 3.10+
- C++17 지원 컴파일러
- pthread

### 빌드 및 실행

```bash
# 빌드
./shl/build.sh

# 서버 실행
./bin/petiq

# 클라이언트 연결 테스트 (다른 터미널에서)
nc localhost 9999
```

### Docker

```bash
# 빌드 & 실행
docker build -t petiq:latest .
docker run -p 9999:9999 --name petiq petiq:latest
```

## 개발 환경

| 항목 | 버전 |
|------|------|
| C++ 표준 | C++17 |
| 빌드 시스템 | CMake 3.10+ |
| 의존성 | pthread |

## 라이선스

이 프로젝트는 교육 및 학습 목적으로 제작되었습니다.

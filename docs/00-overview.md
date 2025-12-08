# Quekka 학습 가이드

## 개요

이 문서는 Quekka 프로젝트를 통해 메시지 큐 시스템을 학습하는 가이드입니다.

## 학습 순서

```
1. API 설계        → 사용자 관점의 인터페이스 설계
2. 프로토콜 직렬화  → 네트워크 통신을 위한 바이트 변환
3. 연결 관리       → TCP 클라이언트 구현
4. 에러 처리       → 견고한 라이브러리 만들기
5. 메모리 관리     → 버퍼와 생명주기
6. 통합 및 테스트   → 전체 시스템 연동
```

## 아키텍처

### 1:1 Producer-Consumer 모델

```
Producer ──(PUBLISH)──► Broker ──(MESSAGE)──► Consumer
                           │
                      Topic Queue
                      [msg1][msg2][...]
```

### 메시지 흐름

1. **Producer → Broker**
   - Producer가 `PUBLISH` 메시지 전송
   - Broker가 토픽 큐에 메시지 저장
   - Broker가 `PUBLISH_ACK` 응답

2. **Broker → Consumer**
   - Consumer가 `SUBSCRIBE` 메시지 전송
   - Broker가 `SUBSCRIBE_ACK` 응답
   - Broker가 새 메시지를 `MESSAGE`로 전달
   - Consumer가 `MESSAGE_ACK` 응답

### 핵심 컴포넌트

| 컴포넌트 | 역할 | 파일 |
|----------|------|------|
| **Broker** | 메시지 중계 서버 | `quekka/src/broker.c` |
| **Topic Manager** | 토픽별 큐 관리 | `quekka/src/topic.c` |
| **Producer** | 메시지 발행 클라이언트 | `producer/src/producer.c` |
| **Consumer** | 메시지 구독 클라이언트 | `consumer/src/consumer.c` |
| **Protocol** | 메시지 포맷 정의 | `common/include/protocol.h` |

## 학습 포인트

### C 언어 시스템 프로그래밍

- 구조체 설계와 메모리 레이아웃
- 포인터와 동적 메모리 관리
- Opaque 타입과 정보 은닉
- 에러 핸들링 패턴

### 네트워크 프로그래밍

- TCP 소켓 생성과 연결
- 논블로킹 I/O
- poll/epoll 이벤트 처리
- 바이트 순서 (엔디안)

### 동시성

- pthread 스레드 생성
- 뮤텍스와 조건 변수
- 프로듀서-컨슈머 패턴

### 프로토콜 설계

- 바이너리 프로토콜 설계
- 메시지 프레이밍
- 요청-응답 패턴

## 다음 단계

[01-api-design.md](01-api-design.md)로 이동하여 API 설계부터 시작하세요.

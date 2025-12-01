# 1단계: API 설계 - 사용자 관점의 인터페이스

## 목표

라이브러리 사용자가 **가장 편하게** 토픽을 발행/구독할 수 있는 C API를 설계합니다.

## 배경

좋은 라이브러리 API의 특징:
- 단순함: 기본 사용법이 3줄 이내
- 일관성: 비슷한 동작은 비슷한 패턴
- 발견 가능성: 함수명만 봐도 용도를 알 수 있음
- 확장성: 나중에 기능 추가해도 기존 코드 안 깨짐

## 과제

### Task 1: 사용자 시나리오 작성

먼저 라이브러리를 **사용하는 입장**에서 코드를 작성하세요.

`examples/` 폴더에 다음 두 파일을 만드세요:

1. **examples/my_publisher.c** - 메시지를 발행하는 예제
2. **examples/my_subscriber.c** - 메시지를 구독하는 예제

이 코드는 컴파일되지 않아도 됩니다. "이렇게 사용하고 싶다"는 희망사항을 코드로 표현하세요.

### Task 2: 헤더 파일 설계

예제 코드가 컴파일될 수 있도록 `libquekka/include/quekka.h`를 작성하세요.

---

## 고민해볼 질문들

API를 설계할 때 다음 질문들을 스스로에게 던져보세요:

### 연결 관리
- [ ] 연결 객체를 어떻게 표현할 것인가? (포인터? 정수 핸들?)
- [ ] 연결 실패 시 어떻게 알려줄 것인가?
- [ ] 하나의 연결로 발행과 구독을 동시에 할 수 있어야 하는가?

### 발행 (Publish)
- [ ] 발행 성공/실패를 어떻게 알려줄 것인가?
- [ ] 발행 후 offset을 반환해야 하는가?
- [ ] 여러 메시지를 한 번에 발행하는 API가 필요한가?

### 구독 (Subscribe)
- [ ] 메시지 수신 방식: 콜백 vs 폴링 vs 둘 다?
- [ ] 콜백을 사용한다면, 어떤 시점에 호출되는가?
- [ ] 콜백 내에서 오래 걸리는 작업을 하면 어떻게 되는가?

### 메시지 구조체
- [ ] 메시지에 어떤 정보가 포함되어야 하는가?
- [ ] 페이로드 메모리는 누가 관리하는가? (라이브러리 vs 사용자)
- [ ] 문자열인가 바이너리인가?

### 에러 처리
- [ ] 에러를 어떻게 전달할 것인가? (반환값? errno? 별도 함수?)
- [ ] 에러 메시지는 어떻게 얻는가?

---

## 참고할 만한 API들

직접 살펴보고 좋은 점을 배워보세요:

- **libcurl**: `curl_easy_init()`, `curl_easy_setopt()`, `curl_easy_perform()`
- **SQLite3**: `sqlite3_open()`, `sqlite3_prepare()`, `sqlite3_step()`
- **librdkafka**: Kafka C 클라이언트 (복잡하지만 완전한 예시)
- **Redis hiredis**: `redisConnect()`, `redisCommand()`

---

## 체크리스트

완료 조건:

- [ ] `examples/my_publisher.c` 작성 완료
- [ ] `examples/my_subscriber.c` 작성 완료
- [ ] `libquekka/include/quekka.h` 작성 완료
- [ ] 헤더 파일만 include하면 예제 코드가 컴파일됨 (링크 에러는 OK)
- [ ] 위의 "고민해볼 질문들"에 대한 나만의 답을 정함

---

## 힌트 (막히면 펼쳐보세요)

<details>
<summary>연결 객체 설계 힌트</summary>

```c
// 방법 1: Opaque 포인터 (권장)
typedef struct quekka_client quekka_client_t;
quekka_client_t* quekka_connect(...);

// 방법 2: 정수 핸들
typedef int quekka_handle_t;
quekka_handle_t quekka_connect(...);
```

Opaque 포인터의 장점:
- 내부 구조 변경해도 사용자 코드 재컴파일 불필요
- NULL 체크로 유효성 검사 가능
</details>

<details>
<summary>메시지 수신 방식 힌트</summary>

```c
// 방법 1: 폴링
quekka_message_t* msg = quekka_poll(client, timeout);

// 방법 2: 콜백
void my_handler(quekka_message_t* msg, void* ctx) { ... }
quekka_subscribe(client, topic, my_handler, user_data);

// 방법 3: 혼합 (poll이 내부에서 콜백 호출)
quekka_subscribe(client, topic, group, handler, ctx);
while (running) {
    quekka_poll(client, 1000);  // 내부에서 handler 호출
}
```
</details>

---

## 다음 단계

API 설계가 완료되면 [02-protocol-serialization.md](02-protocol-serialization.md)로 이동하세요.

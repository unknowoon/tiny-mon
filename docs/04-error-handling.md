# 4단계: 에러 처리 - 견고한 라이브러리 만들기

## 목표

라이브러리 사용자가 **에러 상황을 쉽게 인지하고 적절히 대응**할 수 있는 에러 처리 시스템을 구현합니다.

## 배경

좋은 에러 처리의 특징:
- **감지 가능**: 에러 발생을 놓치지 않음
- **구분 가능**: 에러 종류를 알 수 있음
- **복구 가능**: 적절한 대응 방법을 알 수 있음
- **디버깅 가능**: 원인을 파악할 수 있음

## 과제

### Task 1: 에러 코드 정의

`libquekka/include/quekka.h`에 에러 코드를 정의하세요:

```c
typedef enum {
    QUEKKA_OK = 0,
    QUEKKA_ERR_CONNECTION,    // 연결 실패
    QUEKKA_ERR_TIMEOUT,       // 타임아웃
    QUEKKA_ERR_PROTOCOL,      // 프로토콜 에러
    QUEKKA_ERR_NOT_FOUND,     // 토픽 없음
    QUEKKA_ERR_ALREADY_EXISTS,// 이미 존재
    // ... 필요한 만큼 추가
} quekka_error_t;
```

### Task 2: 에러 정보 저장

클라이언트 구조체에 마지막 에러 정보를 저장하세요:

```c
struct quekka_client {
    int fd;
    // ...
    quekka_error_t last_error;
    char last_error_message[256];
};
```

### Task 3: 에러 조회 API

```c
// 마지막 에러 코드
quekka_error_t quekka_get_error(quekka_client_t *client);

// 에러 메시지 (사람이 읽을 수 있는)
const char* quekka_last_error(quekka_client_t *client);

// 에러 코드 → 문자열
const char* quekka_error_string(quekka_error_t err);
```

---

## 고민해볼 질문들

### 에러 전달 방식
- [ ] 반환값으로 전달? 별도 함수로 조회? 둘 다?
- [ ] `errno` 스타일 vs 명시적 out 파라미터?
- [ ] 성공 시 반환값은? (0? 양수? 포인터?)

### 에러 분류
- [ ] 어떤 에러들이 발생할 수 있는가?
- [ ] 네트워크 에러 vs 프로토콜 에러 vs 사용자 에러?
- [ ] 복구 가능한 에러 vs 치명적 에러?

### 에러 메시지
- [ ] 서버가 보낸 에러 메시지를 그대로 전달?
- [ ] 클라이언트가 추가 정보를 덧붙여야 하는가?
- [ ] 다국어 지원이 필요한가?

### 연결 상태
- [ ] 연결이 끊어진 후 API 호출하면?
- [ ] 자동 재연결? 아니면 에러 반환?
- [ ] 연결 상태를 확인하는 API가 필요한가?

---

## 에러 시나리오별 처리

각 시나리오에서 어떻게 처리할지 결정하세요:

| 시나리오 | 반환값 | 에러 코드 | 메시지 예시 |
|----------|--------|-----------|-------------|
| 연결 실패 | ? | ? | ? |
| 연결 중 타임아웃 | ? | ? | ? |
| 서버가 -ERR 응답 | ? | ? | ? |
| 존재하지 않는 토픽 | ? | ? | ? |
| 잘못된 인자 (NULL) | ? | ? | ? |
| send 중 연결 끊김 | ? | ? | ? |
| recv 중 연결 끊김 | ? | ? | ? |

---

## 체크리스트

- [ ] 에러 코드 enum 정의
- [ ] `quekka_last_error()` 구현
- [ ] `quekka_error_string()` 구현
- [ ] 모든 공개 API에서 에러 발생 시 적절히 기록
- [ ] NULL 포인터 등 잘못된 인자 검사
- [ ] 연결 끊김 상태에서 API 호출 시 적절한 에러 반환

---

## 힌트

<details>
<summary>에러 저장 매크로</summary>

```c
// internal.h
#define SET_ERROR(client, code, fmt, ...) do { \
    (client)->last_error = (code); \
    snprintf((client)->last_error_message, \
             sizeof((client)->last_error_message), \
             fmt, ##__VA_ARGS__); \
} while(0)

// 사용 예
SET_ERROR(client, QUEKKA_ERR_CONNECTION,
          "Failed to connect to %s:%d", host, port);
```
</details>

<details>
<summary>에러 코드 → 문자열</summary>

```c
const char* quekka_error_string(quekka_error_t err) {
    switch (err) {
        case QUEKKA_OK:             return "Success";
        case QUEKKA_ERR_CONNECTION: return "Connection error";
        case QUEKKA_ERR_TIMEOUT:    return "Operation timed out";
        case QUEKKA_ERR_PROTOCOL:   return "Protocol error";
        // ...
        default:                    return "Unknown error";
    }
}
```
</details>

<details>
<summary>NULL 클라이언트 처리</summary>

```c
const char* quekka_last_error(quekka_client_t *client) {
    if (!client) {
        // 전역 에러 메시지 또는 고정 문자열
        return "Invalid client (NULL)";
    }
    return client->last_error_message[0]
           ? client->last_error_message
           : quekka_error_string(client->last_error);
}
```
</details>

---

## 다음 단계

에러 처리가 완료되면 [05-memory-management.md](05-memory-management.md)로 이동하세요.

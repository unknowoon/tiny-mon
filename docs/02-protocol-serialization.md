# 2단계: 프로토콜 직렬화 - 명령어를 바이트로

## 목표

클라이언트 API 호출을 서버가 이해할 수 있는 **바이트 스트림**으로 변환하고, 서버 응답을 다시 C 구조체로 파싱합니다.

## 배경

### Quekka 프로토콜 (README.md 참고)

```
요청: <COMMAND> <args...>\r\n[payload]
응답: +OK <data>\r\n  또는  -ERR <message>\r\n
```

예시:
```
PUBLISH orders 13\r\n{"id": 1234}    → +OK 42\r\n (offset 42에 저장됨)
SUBSCRIBE orders mygroup\r\n         → +OK\r\n
POLL orders mygroup 10 1000\r\n      → +MESSAGES 2\r\n...
```

## 과제

### Task 1: 직렬화 함수 설계

`quekka_publish(client, "orders", data, len)` 호출 시 어떤 바이트가 전송되어야 할까요?

`libquekka/src/protocol.c`에 다음 함수들을 구현하세요:

```c
// 요청 직렬화: C 데이터 → 바이트 버퍼
int protocol_serialize_publish(char *buf, size_t buf_size,
                               const char *topic,
                               const void *payload, size_t payload_len);

int protocol_serialize_subscribe(char *buf, size_t buf_size,
                                 const char *topic, const char *group_id);

int protocol_serialize_poll(char *buf, size_t buf_size,
                            const char *topic, const char *group_id,
                            int max_messages, int timeout_ms);
// ... 기타 명령어들
```

### Task 2: 역직렬화 함수 설계

서버에서 `+OK 42\r\n`가 오면 어떻게 파싱할까요?

```c
// 응답 파싱: 바이트 버퍼 → C 구조체
typedef struct {
    int success;           // 1: OK, 0: ERR
    char *data;            // 응답 데이터 (있으면)
    char *error_message;   // 에러 메시지 (있으면)
} protocol_response_t;

int protocol_parse_response(const char *buf, size_t len,
                            protocol_response_t *response);
```

---

## 고민해볼 질문들

### 버퍼 관리
- [ ] 직렬화 버퍼 크기를 어떻게 정할 것인가?
- [ ] 버퍼가 부족하면 어떻게 처리할 것인가?
- [ ] 동적 할당 vs 정적 버퍼?

### 프로토콜 파싱
- [ ] `\r\n`을 어떻게 찾을 것인가?
- [ ] 불완전한 데이터가 오면? (네트워크에서 잘려서 올 수 있음)
- [ ] 바이너리 페이로드에 `\r\n`이 포함되어 있으면?

### 메시지 포맷
- [ ] POLL 응답으로 여러 메시지가 올 때 포맷은?
- [ ] 각 메시지의 offset, timestamp는 어떻게 전달?

### 엔디안
- [ ] 숫자를 문자열로 보낼 것인가, 바이너리로 보낼 것인가?
- [ ] 바이너리라면 빅엔디안? 리틀엔디안?

---

## 테스트 방법

직렬화 함수를 단독으로 테스트하세요:

```c
// test_protocol.c
void test_serialize_publish() {
    char buf[1024];
    int len = protocol_serialize_publish(buf, sizeof(buf),
                                         "orders", "hello", 5);

    // 예상: "PUBLISH orders 5\r\nhello"
    assert(len == 22);
    assert(memcmp(buf, "PUBLISH orders 5\r\nhello", 22) == 0);
}
```

---

## 체크리스트

- [ ] `libquekka/src/protocol.h` 작성 (내부 헤더)
- [ ] `libquekka/src/protocol.c` 구현
- [ ] 모든 명령어(PUBLISH, SUBSCRIBE, POLL, COMMIT, CREATE 등) 직렬화
- [ ] 응답 파싱 (+OK, -ERR, +MESSAGES, +EMPTY)
- [ ] 단위 테스트 작성 및 통과

---

## 힌트

<details>
<summary>snprintf 활용</summary>

```c
int protocol_serialize_publish(char *buf, size_t buf_size,
                               const char *topic,
                               const void *payload, size_t payload_len) {
    // 헤더 먼저
    int header_len = snprintf(buf, buf_size,
                              "PUBLISH %s %zu\r\n", topic, payload_len);
    if (header_len < 0 || (size_t)header_len >= buf_size) {
        return -1;  // 버퍼 부족
    }

    // 페이로드 복사
    if (header_len + payload_len > buf_size) {
        return -1;
    }
    memcpy(buf + header_len, payload, payload_len);

    return header_len + payload_len;
}
```
</details>

<details>
<summary>응답 파싱 힌트</summary>

```c
int protocol_parse_response(const char *buf, size_t len,
                            protocol_response_t *resp) {
    if (len < 3) return -1;  // 최소 "+OK" or "-ER"

    if (buf[0] == '+') {
        resp->success = 1;
        // "+OK " 이후 데이터 파싱
        // ...
    } else if (buf[0] == '-') {
        resp->success = 0;
        // "-ERR " 이후 에러 메시지 파싱
        // ...
    }
    return 0;
}
```
</details>

---

## 다음 단계

프로토콜 직렬화가 완료되면 [03-connection-management.md](03-connection-management.md)로 이동하세요.

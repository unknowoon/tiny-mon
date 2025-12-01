# 5단계: 메모리 관리 - 버퍼와 생명주기

## 목표

메모리 누수 없이, 사용자가 혼란스럽지 않은 **명확한 메모리 소유권 규칙**을 설계합니다.

## 배경

C 라이브러리에서 메모리 관리는 항상 어렵습니다:
- 누가 할당하고 누가 해제하는가?
- 반환된 포인터는 언제까지 유효한가?
- 콜백에서 받은 데이터를 저장해도 되는가?

## 과제

### Task 1: 소유권 규칙 결정

다음 상황에서 메모리 소유권을 명확히 정의하세요:

```c
// 상황 1: 메시지 발행
int quekka_publish(client, topic, payload, len);
// Q: payload 버퍼는 호출 후에도 유효해야 하는가?

// 상황 2: 메시지 수신 (폴링)
quekka_message_t* quekka_poll_one(client, timeout);
// Q: 반환된 메시지는 누가 해제? 언제까지 유효?

// 상황 3: 메시지 수신 (콜백)
void callback(const quekka_message_t *msg, void *ctx);
// Q: 콜백 함수 종료 후 msg 포인터는 유효한가?

// 상황 4: 토픽 목록
char** quekka_list_topics(client, &count);
// Q: 반환된 문자열 배열은 누가 해제?
```

### Task 2: 수신 버퍼 구현

클라이언트 내부에 수신 버퍼를 구현하세요:

```c
// internal.h
typedef struct {
    char *data;
    size_t capacity;
    size_t length;
    size_t read_pos;
} recv_buffer_t;

struct quekka_client {
    int fd;
    recv_buffer_t recv_buf;
    // ...
};
```

### Task 3: 메시지 풀 (선택사항)

자주 할당/해제되는 메시지 객체를 재사용하는 풀을 구현하세요.

---

## 고민해볼 질문들

### 버퍼 전략
- [ ] 수신 버퍼 초기 크기는?
- [ ] 버퍼가 부족하면 확장? 아니면 에러?
- [ ] 최대 버퍼 크기 제한이 필요한가?

### 메시지 생명주기
- [ ] 메시지 구조체를 라이브러리가 할당? 사용자가 제공?
- [ ] 콜백 방식에서 메시지를 복사해서 저장하고 싶으면?
- [ ] `quekka_message_free()` 같은 해제 함수가 필요한가?

### 문자열 반환
- [ ] 토픽 목록 등 문자열 배열 반환 시 메모리 관리?
- [ ] `quekka_free_topics()` 같은 해제 함수?
- [ ] 아니면 사용자가 버퍼를 제공하는 방식?

### 내부 할당
- [ ] `malloc` 직접 사용? 래퍼 함수?
- [ ] 할당 실패 시 처리 방법?
- [ ] 사용자 정의 할당자 지원? (고급)

---

## 일반적인 패턴들

### 패턴 1: 라이브러리 할당, 사용자 해제

```c
quekka_message_t* msg = quekka_poll_one(client, 1000);
if (msg) {
    process(msg);
    quekka_message_free(msg);  // 명시적 해제
}
```

### 패턴 2: 사용자 버퍼 제공

```c
char topics[10][64];
int count = quekka_list_topics(client, topics, 10, 64);
// 해제 불필요 - 사용자 스택/힙 메모리 사용
```

### 패턴 3: 콜백 내에서만 유효

```c
void on_message(const quekka_message_t *msg, void *ctx) {
    // msg는 이 함수 내에서만 유효
    // 저장하려면 직접 복사해야 함
    save_copy(msg->payload, msg->payload_len);
}
```

### 패턴 4: 이터레이터

```c
quekka_message_t msg;
while (quekka_poll_next(client, &msg)) {
    process(&msg);
    // msg 내부 데이터는 다음 호출 시 덮어씌워짐
}
```

---

## 체크리스트

- [ ] 모든 공개 API에 대해 메모리 소유권 문서화
- [ ] `recv_buffer` 구현 (동적 확장)
- [ ] 필요한 경우 `quekka_xxx_free()` 함수 구현
- [ ] Valgrind로 메모리 누수 검사
- [ ] 대량 메시지 처리 시 메모리 사용량 확인

---

## 힌트

<details>
<summary>동적 버퍼 구현</summary>

```c
int recv_buffer_ensure(recv_buffer_t *buf, size_t needed) {
    if (buf->capacity - buf->length >= needed) {
        return 0;  // 충분함
    }

    size_t new_cap = buf->capacity * 2;
    while (new_cap < buf->length + needed) {
        new_cap *= 2;
    }

    char *new_data = realloc(buf->data, new_cap);
    if (!new_data) return -1;

    buf->data = new_data;
    buf->capacity = new_cap;
    return 0;
}
```
</details>

<details>
<summary>콜백용 메시지 복사</summary>

```c
// 사용자가 콜백에서 메시지를 복사하고 싶을 때
quekka_message_t* quekka_message_dup(const quekka_message_t *src) {
    quekka_message_t *dst = malloc(sizeof(*dst));
    if (!dst) return NULL;

    *dst = *src;  // 기본 필드 복사

    // 페이로드는 별도 할당
    dst->payload = malloc(src->payload_len);
    if (!dst->payload) {
        free(dst);
        return NULL;
    }
    memcpy((void*)dst->payload, src->payload, src->payload_len);

    return dst;
}

void quekka_message_free(quekka_message_t *msg) {
    if (msg) {
        free((void*)msg->payload);
        free(msg);
    }
}
```
</details>

---

## 테스트: Valgrind 사용법

```bash
# 빌드 (디버그 심볼 포함)
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Valgrind 실행
valgrind --leak-check=full --show-leak-kinds=all ./bin/test_client

# 예상 출력 (누수 없음)
# ==12345== HEAP SUMMARY:
# ==12345==     in use at exit: 0 bytes in 0 blocks
# ==12345==   total heap usage: 100 allocs, 100 frees, 10,000 bytes allocated
# ==12345== All heap blocks were freed -- no leaks are possible
```

---

## 다음 단계

메모리 관리가 완료되면 [06-integration-testing.md](06-integration-testing.md)로 이동하세요.

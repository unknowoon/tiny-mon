# 6단계: 통합 및 테스트

## 목표

지금까지 만든 모든 컴포넌트를 **통합**하고, 라이브러리가 **실제로 동작**하는지 검증합니다.

## 배경

개별 모듈은 잘 동작해도 통합하면 문제가 생길 수 있습니다:
- 모듈 간 인터페이스 불일치
- 순서 의존성
- 리소스 경합
- 엣지 케이스

## 과제

### Task 1: 전체 빌드 설정

`libquekka/CMakeLists.txt`를 작성하세요:

```cmake
# 정적 라이브러리로 빌드
add_library(quekka STATIC
    src/quekka.c
    src/connection.c
    src/protocol.c
)

target_include_directories(quekka PUBLIC include)

# 예제 빌드
add_executable(example_publisher examples/my_publisher.c)
target_link_libraries(example_publisher quekka)

add_executable(example_subscriber examples/my_subscriber.c)
target_link_libraries(example_subscriber quekka)
```

### Task 2: 통합 테스트 작성

실제 서버와 통신하는 테스트를 작성하세요:

```c
// tests/test_integration.c
void test_publish_and_consume() {
    // 1. Publisher로 메시지 발행
    quekka_client_t *pub = quekka_connect("127.0.0.1", 9999);
    assert(pub != NULL);

    quekka_create_topic(pub, "test-topic");
    int offset = quekka_publish(pub, "test-topic", "hello", 5);
    assert(offset >= 0);

    // 2. Subscriber로 메시지 수신
    quekka_client_t *sub = quekka_connect("127.0.0.1", 9999);
    quekka_subscribe(sub, "test-topic", "test-group");

    // 3. 메시지 확인
    // ...

    quekka_disconnect(pub);
    quekka_disconnect(sub);
}
```

### Task 3: 엣지 케이스 테스트

다음 상황들을 테스트하세요:

- [ ] 서버가 꺼진 상태에서 연결 시도
- [ ] 연결 중 서버 종료
- [ ] 존재하지 않는 토픽에 발행
- [ ] 빈 토픽에서 폴링
- [ ] 매우 큰 메시지 발행
- [ ] 빠르게 많은 메시지 발행
- [ ] 동시에 여러 클라이언트 연결

---

## 고민해볼 질문들

### 테스트 전략
- [ ] 단위 테스트 vs 통합 테스트의 범위?
- [ ] Mock 서버가 필요한가?
- [ ] CI에서 자동화할 수 있는가?

### 동시성
- [ ] 여러 클라이언트가 동시에 같은 토픽에 발행하면?
- [ ] 라이브러리 자체가 스레드 세이프해야 하는가?
- [ ] 멀티스레드 테스트 방법?

### 성능
- [ ] 처리량 측정 방법?
- [ ] 지연 시간 측정 방법?
- [ ] 병목 지점 파악?

### 문서화
- [ ] API 문서는 어떻게?
- [ ] 예제 코드 품질?
- [ ] README 업데이트?

---

## 테스트 체크리스트

### 기능 테스트

```
[ ] 연결/해제
    [ ] 정상 연결
    [ ] 연결 실패 (서버 없음)
    [ ] 중복 연결
    [ ] 정상 해제
    [ ] 이미 해제된 클라이언트

[ ] 토픽 관리
    [ ] 토픽 생성
    [ ] 중복 생성 시도
    [ ] 토픽 삭제
    [ ] 토픽 목록 조회

[ ] 발행 (Publish)
    [ ] 단일 메시지 발행
    [ ] 빈 메시지 발행
    [ ] 큰 메시지 발행
    [ ] 연속 발행

[ ] 구독 (Subscribe)
    [ ] 구독 등록
    [ ] 메시지 수신
    [ ] 오프셋 커밋
    [ ] 구독 해제

[ ] 에러 처리
    [ ] 잘못된 토픽명
    [ ] NULL 인자
    [ ] 타임아웃
```

### 비기능 테스트

```
[ ] 메모리 누수 (Valgrind)
[ ] 처리량 벤치마크
[ ] 연결 수 한계 테스트
[ ] 장시간 실행 안정성
```

---

## 벤치마크 예제

```c
// benchmark.c
#include <time.h>

void benchmark_publish() {
    quekka_client_t *client = quekka_connect("127.0.0.1", 9999);
    quekka_create_topic(client, "bench");

    const char *msg = "benchmark message payload here";
    int msg_len = strlen(msg);
    int count = 10000;

    clock_t start = clock();

    for (int i = 0; i < count; i++) {
        quekka_publish(client, "bench", msg, msg_len);
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Published %d messages in %.2f seconds\n", count, elapsed);
    printf("Throughput: %.0f msg/sec\n", count / elapsed);

    quekka_disconnect(client);
}
```

---

## 완료 조건

라이브러리가 완성되었다고 판단하는 기준:

1. **예제 동작**: `my_publisher.c`와 `my_subscriber.c`가 실제로 동작
2. **테스트 통과**: 모든 단위/통합 테스트 통과
3. **메모리 안전**: Valgrind에서 누수 없음
4. **문서화**: 모든 공개 API에 주석
5. **빌드**: `cmake && make`로 깔끔하게 빌드

---

## 힌트

<details>
<summary>간단한 테스트 프레임워크</summary>

```c
// test_framework.h
#include <stdio.h>
#include <stdlib.h>

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    printf("Running %s... ", #name); \
    test_##name(); \
    printf("PASS\n"); \
} while(0)

#define ASSERT(cond) do { \
    if (!(cond)) { \
        printf("FAIL\n  Assertion failed: %s\n  At %s:%d\n", \
               #cond, __FILE__, __LINE__); \
        exit(1); \
    } \
} while(0)

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NE(a, b) ASSERT((a) != (b))
#define ASSERT_NULL(p) ASSERT((p) == NULL)
#define ASSERT_NOT_NULL(p) ASSERT((p) != NULL)
```

```c
// test_main.c
#include "test_framework.h"
#include "quekka.h"

TEST(connect_success) {
    quekka_client_t *c = quekka_connect("127.0.0.1", 9999);
    ASSERT_NOT_NULL(c);
    quekka_disconnect(c);
}

TEST(connect_fail_no_server) {
    quekka_client_t *c = quekka_connect("127.0.0.1", 9998);
    ASSERT_NULL(c);
}

int main() {
    RUN_TEST(connect_success);
    RUN_TEST(connect_fail_no_server);
    printf("\nAll tests passed!\n");
    return 0;
}
```
</details>

---

## 축하합니다!

여기까지 완료했다면 **동작하는 클라이언트 라이브러리**를 만든 것입니다.

### 추가 학습 주제 (선택)

- **비동기 API**: `poll()` 대신 이벤트 기반
- **스레드 안전성**: 멀티스레드 환경 지원
- **연결 풀링**: 여러 연결 관리
- **배치 처리**: 여러 메시지 한 번에 전송
- **압축**: 페이로드 압축 옵션
- **TLS**: 암호화 통신

### 학습 회고

각 단계에서 배운 점을 `docs/retrospective.md`에 기록해보세요:

- 어떤 설계 결정이 좋았는가?
- 다시 한다면 다르게 할 부분은?
- 가장 어려웠던 부분은?
- 가장 재미있었던 부분은?

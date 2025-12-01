# 3단계: 연결 관리 - TCP 클라이언트 구현

## 목표

서버에 TCP 연결을 맺고, 데이터를 송수신하는 **클라이언트 측 네트워크 코드**를 구현합니다.

## 배경

서버(`socket_server.c`)는 이미 구현되어 있습니다. 이제 클라이언트가 필요합니다.

```
┌──────────────┐          TCP          ┌──────────────┐
│  libquekka   │  ←───────────────────→  │   Quekka     │
│  (클라이언트) │     요청/응답          │   Server     │
└──────────────┘                        └──────────────┘
```

## 과제

### Task 1: 기본 연결

`libquekka/src/connection.c`에 TCP 클라이언트를 구현하세요:

```c
// 서버에 연결
int connection_connect(const char *host, int port);

// 연결 종료
void connection_close(int fd);

// 데이터 전송 (전체가 전송될 때까지)
int connection_send(int fd, const char *data, size_t len);

// 데이터 수신 (\r\n까지 읽기 또는 지정된 바이트만큼)
int connection_recv_line(int fd, char *buf, size_t buf_size);
int connection_recv_bytes(int fd, char *buf, size_t len);
```

### Task 2: quekka_client 구조체

`libquekka/src/internal.h`에 클라이언트 내부 상태를 정의하세요:

```c
struct quekka_client {
    int fd;                    // 소켓 파일 디스크립터
    char host[256];            // 서버 호스트
    int port;                  // 서버 포트
    // ... 추가로 필요한 것들
};
```

### Task 3: 공개 API 연결

`libquekka/src/quekka.c`에서 공개 API와 내부 구현을 연결하세요:

```c
quekka_client_t* quekka_connect(const char *host, int port) {
    quekka_client_t *client = malloc(sizeof(*client));
    // connection_connect 호출
    // 구조체 초기화
    return client;
}
```

---

## 고민해볼 질문들

### 소켓 생성
- [ ] IPv4만? IPv6도?
- [ ] `getaddrinfo()` vs 직접 `inet_pton()`?
- [ ] 연결 타임아웃은 어떻게 설정?

### 데이터 송신
- [ ] `send()`가 전체를 보내지 않으면? (partial write)
- [ ] 연결이 끊어진 상태에서 send하면?
- [ ] `SIGPIPE` 시그널 처리?

### 데이터 수신
- [ ] `recv()`가 원하는 만큼 읽지 못하면? (partial read)
- [ ] 응답의 끝을 어떻게 알 수 있는가?
- [ ] 타임아웃 처리? (`select()` 또는 `setsockopt()`)

### 연결 상태
- [ ] 연결이 끊어진 것을 어떻게 감지?
- [ ] 재연결 로직이 필요한가?
- [ ] 여러 스레드에서 하나의 연결을 공유해도 되는가?

---

## 테스트 방법

### 1. 에코 서버로 테스트

```bash
# 터미널 1: 에코 서버 실행
./bin/example_echo 9999

# 터미널 2: 클라이언트 테스트
./bin/test_connection
```

### 2. netcat으로 수동 테스트

```bash
# 서버 역할 (테스트용)
nc -l 9999

# 다른 터미널에서 클라이언트 실행
./bin/test_connection
```

---

## 체크리스트

- [ ] `libquekka/src/connection.h` 작성
- [ ] `libquekka/src/connection.c` 구현
- [ ] `connection_connect()` - 서버 연결
- [ ] `connection_send()` - partial write 처리
- [ ] `connection_recv_line()` - `\r\n`까지 읽기
- [ ] `connection_recv_bytes()` - 정확히 N바이트 읽기
- [ ] `quekka_connect()` / `quekka_disconnect()` 동작 확인

---

## 힌트

<details>
<summary>TCP 연결 기본 코드</summary>

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int connection_connect(const char *host, int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        close(fd);
        return -1;
    }

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}
```
</details>

<details>
<summary>Partial write 처리</summary>

```c
int connection_send(int fd, const char *data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, data + sent, len - sent, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR) continue;  // 인터럽트, 재시도
            return -1;  // 실제 에러
        }
        if (n == 0) return -1;  // 연결 끊김
        sent += n;
    }
    return (int)sent;
}
```
</details>

<details>
<summary>Line 단위 수신</summary>

```c
// 간단한 버전 (비효율적이지만 이해하기 쉬움)
int connection_recv_line(int fd, char *buf, size_t buf_size) {
    size_t pos = 0;
    while (pos < buf_size - 1) {
        char c;
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0) return -1;

        buf[pos++] = c;
        if (pos >= 2 && buf[pos-2] == '\r' && buf[pos-1] == '\n') {
            buf[pos] = '\0';
            return pos;
        }
    }
    return -1;  // 버퍼 초과
}
```

> 주의: 바이트 단위 recv는 성능이 나쁩니다. 실제로는 버퍼링이 필요합니다.
</details>

---

## 다음 단계

연결 관리가 완료되면 [04-error-handling.md](04-error-handling.md)로 이동하세요.

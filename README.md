# Tiny-Mon

TCP 서버 기반의 모니터링 시스템

## 프로젝트 구조

```
tiny-mon/
├── CMakeLists.txt      # CMake 빌드 설정
├── build.sh            # 빌드 스크립트
├── clean.sh            # 클린 스크립트
├── bin/                # 빌드된 실행 파일
├── libsrc/             # 라이브러리 소스
│   ├── inc/
│   │   └── logger.h    # 로거 헤더
│   └── libLogger.c     # 로거 구현
└── src/                # 애플리케이션 소스
    ├── comm.h          # 통신 헤더
    ├── comm.c          # 클라이언트 핸들러
    └── commManager.c   # 메인 서버
```

## 요구사항

- CMake 3.10 이상
- C99 지원 컴파일러 (gcc, clang 등)
- pthread 라이브러리

### macOS 설치

```bash
brew install cmake
```

### Ubuntu/Debian 설치

```bash
sudo apt-get update
sudo apt-get install cmake build-essential
```

### CentOS/RHEL 설치

```bash
sudo yum install cmake gcc make
```

## 빌드 방법

### 스크립트 사용 (권장)

```bash
# Debug 빌드 (기본값)
./build.sh

# Release 빌드
./build.sh Release
```

### 수동 빌드

```bash
# 빌드 디렉토리 생성 및 이동
mkdir build && cd build

# CMake 설정
cmake -DCMAKE_BUILD_TYPE=Debug ..

# 빌드
make -j$(nproc)

# 실행 파일이 bin/ 디렉토리에 생성됩니다
```

## 실행 방법

### 서버 시작

```bash
# 기본 포트(8080) 사용
./bin/commManager

# 사용자 지정 포트 사용
./bin/commManager 9090
```

### 클라이언트 연결 테스트

다른 터미널에서:

```bash
# telnet으로 연결
telnet localhost 8080

# 또는 netcat 사용
nc localhost 8080
```

메시지를 입력하면 서버가 에코 응답을 보냅니다.
`quit`를 입력하면 연결이 종료됩니다.

## 클린

```bash
# 빌드 파일만 삭제
./clean.sh

# 빌드 파일 + 로그 파일 삭제
./clean.sh --full
```

## 주요 기능

### Logger 라이브러리

- 5가지 로그 레벨 지원 (DEBUG, INFO, WARNING, ERROR, FATAL)
- 컬러 콘솔 출력
- 파일 출력 (UTF-8 인코딩)
- Thread-safe (pthread mutex 사용)
- 로그 레벨별 필터링

### CommManager (서버)

- TCP 소켓 서버
- 멀티 프로세스 아키텍처 (fork 기반)
- 클라이언트당 별도 프로세스 생성
- 로그 파일: `commmanager.log`

### Comm (클라이언트 핸들러)

- 클라이언트 연결 처리
- 에코 서버 기능
- 프로세스별 로그 파일: `comm_<pid>.log`

## CMake 옵션

### 빌드 타입

```bash
# Debug (디버그 심볼, 최적화 없음)
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release (최적화 -O3, 디버그 심볼 없음)
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### 컴파일러 지정

```bash
# gcc 사용
cmake -DCMAKE_C_COMPILER=gcc ..

# clang 사용
cmake -DCMAKE_C_COMPILER=clang ..
```

## 로그 파일

실행 중 생성되는 로그 파일:
- `commmanager.log` - 서버 메인 프로세스 로그
- `comm_<pid>.log` - 각 클라이언트 핸들러 프로세스 로그

## 개발 정보

- C 표준: C99
- 빌드 시스템: CMake 3.10+
- 의존성: pthread

## 문제 해결

### CMake를 찾을 수 없음

CMake가 설치되어 있는지 확인:
```bash
cmake --version
```

### 빌드 오류

1. 빌드 디렉토리를 삭제하고 다시 시도:
   ```bash
   ./clean.sh
   ./build.sh
   ```

2. 컴파일러가 설치되어 있는지 확인:
   ```bash
   gcc --version
   # 또는
   clang --version
   ```

### 포트가 이미 사용 중

다른 포트 번호를 지정:
```bash
./bin/commManager 9090
```

## 라이선스

이 프로젝트는 교육 및 학습 목적으로 제작되었습니다.

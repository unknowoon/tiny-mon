# ============================================
# Stage 1: Build
# ============================================
FROM rockylinux:9 AS builder

# 빌드 도구 설치
RUN dnf -y update && \
    dnf -y install gcc gcc-c++ cmake make && \
    dnf clean all

WORKDIR /app

# 소스 복사
COPY CMakeLists.txt .
COPY src/ src/
COPY libsrc/ libsrc/

# 빌드
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# ============================================
# Stage 2: Runtime
# ============================================
FROM rockylinux:9-minimal

# 런타임 필수 라이브러리만 설치
RUN microdnf -y install libstdc++ && \
    microdnf clean all

WORKDIR /app

# 빌드된 바이너리만 복사
COPY --from=builder /app/bin/petiq .

# 로그 디렉토리 생성
RUN mkdir -p /app/logs

# 서버 포트 노출
EXPOSE 9999

# 실행
CMD ["./petiq"]
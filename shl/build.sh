#!/bin/bash

# Quekka 빌드 스크립트

set -e  # 에러 발생 시 중단

# 색상 정의
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== Quekka 빌드 스크립트 ===${NC}"

# CMake 설치 확인
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}오류: CMake가 설치되어 있지 않습니다.${NC}"
    echo "CMake를 설치해주세요:"
    echo "  macOS: brew install cmake"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  CentOS/RHEL: sudo yum install cmake"
    exit 1
fi

# 빌드 타입 설정 (기본값: Debug)
BUILD_TYPE=${1:-Debug}

echo -e "${YELLOW}빌드 타입: ${BUILD_TYPE}${NC}"

# 빌드 디렉토리 생성
if [ -d "build" ]; then
    echo -e "${YELLOW}기존 빌드 디렉토리를 삭제합니다...${NC}"
    rm -rf build
fi

mkdir -p build
cd build

# CMake 설정
echo -e "${GREEN}CMake 설정 중...${NC}"
cmake -DCMAKE_BUILD_TYPE=${BUILD_TYPE} ..

# 빌드
echo -e "${GREEN}빌드 중...${NC}"
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo -e "${GREEN}=== 빌드 완료! ===${NC}"
echo -e "실행 파일 위치:"
echo -e "  - commManager: ${GREEN}bin/commManager${NC}"
echo -e "  - comm: ${GREEN}bin/comm${NC}"
echo ""
echo -e "실행 방법:"
echo -e "  ${YELLOW}./bin/commManager [포트번호]${NC}"
echo ""

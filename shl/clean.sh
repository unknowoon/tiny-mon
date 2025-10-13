#!/bin/bash

# Tiny-Mon 클린 스크립트

# 색상 정의
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Tiny-Mon 클린 스크립트 ===${NC}"

# 빌드 디렉토리 삭제
if [ -d "build" ]; then
    echo -e "${YELLOW}빌드 디렉토리 삭제 중...${NC}"
    rm -rf build
    echo -e "${GREEN}빌드 디렉토리 삭제 완료${NC}"
else
    echo -e "${YELLOW}빌드 디렉토리가 존재하지 않습니다.${NC}"
fi

# bin 디렉토리의 실행 파일 삭제
if [ -d "bin" ]; then
    echo -e "${YELLOW}bin 디렉토리 클린 중...${NC}"
    rm -f bin/commManager bin/comm bin/*.a bin/*.so
    echo -e "${GREEN}bin 디렉토리 클린 완료${NC}"
else
    echo -e "${YELLOW}bin 디렉토리가 존재하지 않습니다.${NC}"
fi

# 로그 파일 삭제 (선택사항)
if [ "$1" == "--full" ] || [ "$1" == "-f" ]; then
    echo -e "${YELLOW}로그 파일 삭제 중...${NC}"
    rm -f *.log
    echo -e "${GREEN}로그 파일 삭제 완료${NC}"
fi

echo -e "${GREEN}=== 클린 완료! ===${NC}"

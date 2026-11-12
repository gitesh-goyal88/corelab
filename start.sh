
#!/bin/bash

set -e

echo "===================================="
echo "       CORELAB BACKEND STARTER"
echo "===================================="

# Navigate to project root
cd "$(dirname "$0")"

# Check dependencies
if ! command -v g++ >/dev/null 2>&1; then
    echo "Installing build tools..."
    sudo apt update
    sudo apt install -y build-essential make
fi

# Build backend
echo ""
echo "[1/2] Building CoreLab..."

make clean
make

# Start backend
echo ""
echo "[2/2] Starting CoreLab..."

echo "Backend: http://localhost:8080"
echo "Press Ctrl+C to stop."

./corelab 8080


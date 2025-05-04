#!/usr/bin/env bash
set -e

declare -A cases=(
  ["1+2*3"]=7
  ["(2+3)*4"]=20
  ["sqrt(25)+3"]=8
  ["sin(3.14159265/2)"]=1
)

bin="./build/script.hulk"

echo "Ejecutando tests..."
for expr in "${!cases[@]}"; do
  out=$(echo "$expr" | $bin)
  if [[ "$out" == "${cases[$expr]}" ]]; then
    echo " OK: $expr → $out"
  else
    echo "FAIL: $expr → $out (esperado ${cases[$expr]})"
    exit 1
  fi
done

echo "Todos los tests pasaron ✔"

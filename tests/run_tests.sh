#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# run_tests.sh – Ejecuta cada input*.hulk y compara con el resultado esperado
# -----------------------------------------------------------------------------
SCRIPT="../build/script.hulk.exe"
pass=0
fail=0

for f in input*.hulk; do
  # Define aquí el valor o el código de error esperado para cada test
  case "$f" in
    input1.hulk)   exp="20"       ;;
    input2.hulk)   exp="20"       ;;
    input3.hulk)   exp="4"        ;;
    input4.hulk)   exp="-0.6"     ;;
    input5.hulk)   exp="42"       ;;
    input6.hulk)   exp_err=1      ;;  # división por cero → error
    input7.hulk)   exp="3"        ;;
    input8.hulk)   exp="1"        ;;  # sin(pi/2) ≈ 1
    input9.hulk)   exp_err=1      ;;  # sintaxis inválida
    input10.hulk)  exp_err=1      ;;  # sintaxis inválida
    *) echo "[Test $f] ¡Sin expectativa definida!"; continue ;;
  esac

  echo -n "[Test $f] "
  if [[ -n "$exp" ]]; then
    out=$($SCRIPT "$f" 2>/dev/null)
    if [[ "$out" == "$exp" ]]; then
      echo "OK → $out"
      pass=$((pass+1))
    else
      echo "FAIL → got '$out', expected '$exp'"
      fail=$((fail+1))
    fi
  else
    $SCRIPT "$f" >/dev/null 2>&1
    code=$?
    if [[ $code -ne 0 ]]; then
      echo "OK (error code $code)"
      pass=$((pass+1))
    else
      echo "FAIL → expected error, got exit 0"
      fail=$((fail+1))
    fi
  fi

  # Limpia variables para el siguiente test
  unset exp exp_err
done

echo
echo "Resumen: $pass pasaron, $fail fallaron."
exit $fail

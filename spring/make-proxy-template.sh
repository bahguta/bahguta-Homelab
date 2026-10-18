#!/usr/bin/env bash
# Genera haproxy.cfg.template para docker-socket-proxy.
#
# En la imagen, la regla que deniega todo POST (cuando POST=0) va ANTES que las
# reglas ALLOW_START / ALLOW_STOP / ALLOW_RESTARTS, asi que start y stop quedan
# bloqueados. Este script copia la plantilla de la imagen y mueve esas reglas
# antes del bloqueo. NO se usa POST=1, porque permitiria crear contenedores.
#
# Uso (en esta carpeta):  bash make-proxy-template.sh
# Despues:                docker compose up -d
# Repetir si actualizas la imagen del proxy.
set -euo pipefail
cd "$(dirname "$0")"

IMAGE="tecnativa/docker-socket-proxy"
SRC="/usr/local/etc/haproxy/haproxy.cfg.template"
OUT="haproxy.cfg.template"
ALLOW='env\(ALLOW_(RESTARTS|START|STOP)\)'
DENY='deny unless METH_GET'

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

docker run --rm --entrypoint cat "$IMAGE" "$SRC" > "$tmp/orig"

if [ "$(grep -cE "$ALLOW" "$tmp/orig")" -ne 3 ] || [ "$(grep -c "$DENY" "$tmp/orig")" -ne 1 ]; then
  echo "ERROR: la plantilla de la imagen tiene un formato distinto al esperado." >&2
  echo "Revisa $SRC dentro de $IMAGE (reglas ALLOW_* y '$DENY')." >&2
  exit 1
fi

grep -E "$ALLOW" "$tmp/orig" > "$tmp/allow"
grep -vE "$ALLOW" "$tmp/orig" > "$tmp/rest"
awk -v deny="$DENY" 'FNR==NR { a[++n]=$0; next } index($0, deny) { for (i=1;i<=n;i++) print a[i] } { print }' \
  "$tmp/allow" "$tmp/rest" > "$OUT"

# Comprobacion: las tres reglas ALLOW_* deben quedar antes del bloqueo
deny_line="$(grep -n "$DENY" "$OUT" | head -1 | cut -d: -f1)"
last_allow="$(grep -nE "$ALLOW" "$OUT" | tail -1 | cut -d: -f1)"
if [ "$last_allow" -ge "$deny_line" ]; then
  echo "ERROR: el orden de las reglas no es el esperado." >&2
  exit 1
fi

echo "OK: $OUT generado (reglas ALLOW_* en las lineas anteriores a la $deny_line)."
echo "Ahora ejecuta: docker compose up -d"

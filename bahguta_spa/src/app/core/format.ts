/** Dirección del viento en grados → punto cardinal. */
export const compass = (degrees: number) =>
  ['N', 'NE', 'E', 'SE', 'S', 'SO', 'O', 'NO'][Math.round(degrees / 45) % 8];

/** Hora local (HH:mm) de un instante, aplicando el desfase horario (segundos) del lugar. */
export const hm = (ts: number, tz: number) => new Date((ts + tz) * 1000).toISOString().slice(11, 16);

const WEEKDAY = new Intl.DateTimeFormat('es-ES', { weekday: 'long', timeZone: 'UTC' });

/** "2026-10-21" → "Miércoles". */
export const weekday = (date: string) => {
  const w = WEEKDAY.format(new Date(`${date}T00:00:00Z`));
  return w.charAt(0).toUpperCase() + w.slice(1);
};

/** "2026-10-21" → "21/10". */
export const dayMonth = (date: string) => `${date.slice(8, 10)}/${date.slice(5, 7)}`;

/** Segundos → "1 d 3 h" o "3 h 20 min". */
export function uptime(seconds: number): string {
  const d = Math.floor(seconds / 86400);
  const h = Math.floor((seconds % 86400) / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  return d > 0 ? `${d} d ${h} h` : `${h} h ${m} min`;
}

/** Color según el nivel de uso: ok, warn (≥ warn) o danger (≥ danger). */
export const level = (v: number, warn = 75, danger = 90): 'ok' | 'warn' | 'danger' =>
  v >= danger ? 'danger' : v >= warn ? 'warn' : 'ok';

/** Segundos → "30 min", "2 h" o "45 s". */
export function every(seconds: number): string {
  if (seconds >= 3600 && seconds % 3600 === 0) return `${seconds / 3600} h`;
  if (seconds >= 60 && seconds % 60 === 0) return `${seconds / 60} min`;
  return `${seconds} s`;
}

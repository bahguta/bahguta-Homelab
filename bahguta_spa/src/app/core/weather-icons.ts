import { signal } from '@angular/core';

/** Códigos de icono de OpenWeather (p. ej. "04n") → emoji. */
export function weatherEmoji(code: string | undefined): string {
  if (!code) return '⛅';
  const night = code.endsWith('n');
  switch (code.slice(0, 2)) {
    case '01': return night ? '🌙' : '☀️';
    case '02': return night ? '☁️' : '🌤️';
    case '03':
    case '04': return '☁️';
    case '09': return '🌧️';
    case '10': return '🌦️';
    case '11': return '⛈️';
    case '13': return '❄️';
    case '50': return '🌫️';
    default: return '⛅';
  }
}

/** [día, noche] → SVG animado de Meteocons (public/meteocons). */
const ICONS: Record<string, [string, string]> = {
  '01': ['clear-day', 'clear-night'],
  '02': ['partly-cloudy-day', 'partly-cloudy-night'],
  '03': ['cloudy', 'cloudy'],
  '04': ['overcast', 'overcast-night'],
  '09': ['rain', 'rain'],
  '10': ['partly-cloudy-day-rain', 'partly-cloudy-night-rain'],
  '11': ['thunderstorms-rain', 'thunderstorms-rain'],
  '13': ['snow', 'snow'],
  '50': ['mist', 'mist'],
};

/** Código de OpenWeather → ruta del icono animado. */
export function weatherIcon(code: string | undefined): string {
  const pair = (code && ICONS[code.slice(0, 2)]) || ICONS['03'];
  return `/meteocons/${pair[code?.endsWith('n') ? 1 : 0]}.svg`;
}

/** Parámetros de la animación de fondo (WeatherBg). */
export interface WeatherScene {
  /** Cobertura de nubes, 0–1. */
  cover: number;
  /** Intensidad de la lluvia, 0–1. */
  rain: number;
  lightning: boolean;
  night: boolean;
}

const SCENES: Record<string, Omit<WeatherScene, 'night'>> = {
  '02': { cover: 0.35, rain: 0, lightning: false },
  '03': { cover: 0.55, rain: 0, lightning: false },
  '04': { cover: 0.8, rain: 0, lightning: false },
  '09': { cover: 0.9, rain: 0.7, lightning: false },
  '10': { cover: 0.75, rain: 0.4, lightning: false },
  '11': { cover: 1, rain: 1, lightning: true },
};

/** Escenas que se pueden forzar para previsualizar (id → código base de OpenWeather). */
const FORCED: Record<string, string> = {
  storm: '11',
  rain: '09',
  drizzle: '10',
  overcast: '04',
  cloudy: '03',
  partly: '02',
  clear: '01',
};

export const SCENE_OPTIONS = [
  { id: 'storm', label: 'Tormenta' },
  { id: 'rain', label: 'Lluvia' },
  { id: 'drizzle', label: 'Llovizna' },
  { id: 'overcast', label: 'Cubierto' },
  { id: 'cloudy', label: 'Nublado' },
  { id: 'partly', label: 'Poco nuboso' },
  { id: 'clear', label: 'Despejado' },
];

export interface SceneOverride {
  name: string;
  night: boolean;
}

/** Vista previa de una escena en lugar del tiempo real (null = tiempo real). Solo vive en memoria. */
const [urlName, urlSuffix] = (new URLSearchParams(globalThis.location?.search ?? '').get('scene') ?? '').split('-');
export const sceneOverride = signal<SceneOverride | null>(
  FORCED[urlName] ? { name: urlName, night: urlSuffix === 'night' } : null,
);

/** Código efectivo: el de la vista previa si la hay, o el real. */
function effectiveCode(code: string | undefined): string | undefined {
  const o = sceneOverride();
  return o ? FORCED[o.name] + (o.night ? 'n' : 'd') : code;
}

/** ¿Es de noche? (respeta la vista previa). */
export function weatherNight(code: string | undefined): boolean {
  return !!effectiveCode(code)?.endsWith('n');
}

/**
 * Código de OpenWeather → animación de fondo. Siempre hay una (de día con sol, de noche con luna).
 * `real` ignora la vista previa: se usa en las tarjetas de la previsión, que muestran su propio tiempo.
 */
export function weatherScene(code: string | undefined, real = false): WeatherScene | null {
  const c = real ? code : effectiveCode(code);
  if (!c) return null;
  const key = c.slice(0, 2);
  const s = SCENES[key] ?? { cover: key === '01' ? 0 : 0.35, rain: 0, lightning: false };
  return { ...s, night: c.endsWith('n') };
}

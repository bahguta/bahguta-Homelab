import { Component, DestroyRef, computed, inject, signal } from '@angular/core';
import { DecimalPipe } from '@angular/common';
import { DashboardService } from '../../core/dashboard.service';
import { compass, dayMonth, hm, weekday } from '../../core/format';
import { SCENE_OPTIONS, sceneOverride, weatherIcon, weatherNight, weatherScene } from '../../core/weather-icons';
import { WeatherBg } from '../../shared/weather-bg/weather-bg';

@Component({
  selector: 'app-weather',
  imports: [DecimalPipe, WeatherBg],
  templateUrl: './weather.html',
  styleUrl: './weather.scss',
})
export class Weather {
  private readonly dashboard = inject(DashboardService);

  /** Hora actual (epoch, segundos). Se actualiza cada 30 s para mover la barra del sol. */
  private readonly now = signal(Math.floor(Date.now() / 1000));

  constructor() {
    const timer = setInterval(() => this.now.set(Math.floor(Date.now() / 1000)), 30_000);
    inject(DestroyRef).onDestroy(() => clearInterval(timer));
  }

  protected readonly override = sceneOverride;
  protected readonly sceneOptions = SCENE_OPTIONS;

  /** Elige una escena para previsualizar ('' = tiempo real). */
  protected setScene(id: string): void {
    this.override.set(id ? { name: id, night: this.vm()?.night ?? false } : null);
  }

  protected toggleNight(): void {
    this.override.update((o) => (o ? { ...o, night: !o.night } : o));
  }

  protected readonly vm = computed(() => {
    const d = this.dashboard.data();
    const w = d?.weather?.ok ? d.weather.data : undefined;
    if (!d || !w) return null;

    const c = w.current;
    const today = new Date((d.ts + c.timezone) * 1000).toISOString().slice(0, 10);

    // Progreso del día: 0 % al amanecer, 100 % al atardecer (según la hora actual).
    const now = this.now();
    const sunSpan = c.sunset - c.sunrise;
    const sunRaw = sunSpan > 0 ? ((now - c.sunrise) / sunSpan) * 100 : 0;
    const isDay = now >= c.sunrise && now <= c.sunset;

    // Escala común de la semana para pintar el rango min–max de cada día.
    const weekMin = Math.min(...w.forecast.map((f) => f.min));
    const weekMax = Math.max(...w.forecast.map((f) => f.max));
    const span = Math.max(weekMax - weekMin, 1);

    return {
      c,
      night: weatherNight(c.icon),
      icon: weatherIcon(c.icon),
      scene: weatherScene(c.icon),
      wind: `${(c.windSpeed * 3.6).toFixed(0)} km/h ${compass(c.windDirection)}`,
      sunrise: hm(c.sunrise, c.timezone),
      sunset: hm(c.sunset, c.timezone),
      isDay,
      sunPct: Math.min(100, Math.max(0, sunRaw)),
      days: w.forecast.map((f) => ({
        name: f.date === today ? 'Hoy' : weekday(f.date),
        date: dayMonth(f.date),
        scene: weatherScene(f.icon, true),
        description: f.description,
        min: f.min,
        max: f.max,
        rain: f.rainMm,
        left: ((f.min - weekMin) / span) * 100,
        width: Math.max(((f.max - f.min) / span) * 100, 4),
        slots: f.hours.map((h) => ({ hour: `${h.hour}:00`, icon: weatherIcon(h.icon), temp: h.max })),
      })),
    };
  });
}

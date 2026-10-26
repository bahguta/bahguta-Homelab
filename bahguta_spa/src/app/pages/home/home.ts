import { Component, computed, inject } from '@angular/core';
import { DecimalPipe } from '@angular/common';
import { RouterLink } from '@angular/router';
import { DashboardService } from '../../core/dashboard.service';
import { compass, hm, uptime } from '../../core/format';
import { weatherIcon, weatherNight, weatherScene } from '../../core/weather-icons';
import { WeatherBg } from '../../shared/weather-bg/weather-bg';

interface Card {
  route: string;
  icon: string;
  title: string;
  value: string;
  detail: string;
  lines: string[];
  /** 0-100: pinta una barra de progreso en la tarjeta. */
  progress?: number;
}

const DASH = '—';
const deg = (v: number | null | undefined) => (v == null ? DASH : `${v.toFixed(1)}°`);
const pct = (v: number | null | undefined) => (v == null ? DASH : `${v.toFixed(0)}%`);

@Component({
  selector: 'app-home',
  imports: [RouterLink, DecimalPipe, WeatherBg],
  templateUrl: './home.html',
  styleUrl: './home.scss',
})
export class Home {
  private readonly dashboard = inject(DashboardService);

  protected readonly loaded = computed(() => this.dashboard.data() !== null);

  protected readonly weather = computed(() => {
    const d = this.dashboard.data();
    const w = d?.weather?.ok ? d.weather.data : undefined;
    if (!d || !w) return null;

    const c = w.current;

    return {
      c,
      night: weatherNight(c.icon),
      icon: weatherIcon(c.icon),
      scene: weatherScene(c.icon),
      wind: `${(c.windSpeed * 3.6).toFixed(0)} km/h ${compass(c.windDirection)}`,
      sunrise: hm(c.sunrise, c.timezone),
      sunset: hm(c.sunset, c.timezone),
    };
  });

  protected readonly cards = computed<Card[]>(() => {
    const d = this.dashboard.data();
    const rooms = d?.rooms?.ok ? d.rooms.data : undefined;
    const sys = d?.system?.ok ? d.system.data : undefined;
    const red = d?.red?.ok ? d.red.data : undefined;
    const g = d?.grafana?.ok ? d.grafana.data : undefined;
    const s = d?.summary;

    const roomsWithData = rooms?.filter((r) => !r.stale).length;
    const dockerPct = s && s.docker.total ? ((s.docker.running ?? 0) / s.docker.total) * 100 : undefined;
    const diskPct = g ? (g.rootUsedGb / g.rootTotalGb) * 100 : undefined;

    return [
      {
        route: '/casa',
        icon: '🛋️',
        title: 'Casa',
        value: deg(s?.avgTemperature),
        detail: 'temperatura media',
        lines: [
          `Humedad media ${pct(s?.avgHumidity)}`,
          rooms ? `${roomsWithData}/${rooms.length} habitaciones con datos` : '',
        ].filter(Boolean),
      },
      {
        route: '/docker',
        icon: '🐳',
        title: 'Docker',
        value: s ? `${s.docker.running}/${s.docker.total}` : DASH,
        detail: 'contenedores en marcha',
        lines: s ? [`${s.docker.total - (s.docker.running ?? 0)} parados`] : [],
        progress: dockerPct,
      },
      {
        route: '/devices',
        icon: '📟',
        title: 'Devices',
        value: s ? `${s.devices.online}/${s.devices.total}` : DASH,
        detail: 'dispositivos online',
        lines: s ? [`${s.devices.total - (s.devices.online ?? 0)} offline`] : [],
      },
      {
        route: '/system',
        icon: '🖥️',
        title: 'System',
        value: pct(sys?.cpu),
        detail: 'CPU',
        lines: sys ? [`RAM ${pct(sys.ram)}`, `Uptime ${uptime(sys.uptime)}`] : [],
        progress: sys?.cpu,
      },
      {
        route: '/red',
        icon: '📶',
        title: 'Red',
        value: red ? String(red.total) : DASH,
        detail: 'equipos conectados',
        lines: red ? red.clients.slice(0, 3).map((c) => c.name) : [],
      },
      {
        route: '/grafana',
        icon: '📊',
        title: 'Grafana',
        value: pct(diskPct),
        detail: 'disco /',
        lines: g
          ? [
              `${g.rootUsedGb.toFixed(0)} / ${g.rootTotalGb.toFixed(0)} GB`,
              g.storageTotalGb ? `Storage ${g.storageUsedGb?.toFixed(0)} / ${g.storageTotalGb.toFixed(0)} GB` : '',
              `Temperatura ${g.tempC.toFixed(0)} °C`,
            ].filter(Boolean)
          : [],
        progress: diskPct,
      },
    ];
  });
}

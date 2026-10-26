import { Component, computed, inject } from '@angular/core';
import { DashboardService } from '../../core/dashboard.service';
import { every, level, uptime } from '../../core/format';

@Component({
  selector: 'app-system',
  templateUrl: './system.html',
  styleUrl: './system.scss',
})
export class System {
  private readonly dashboard = inject(DashboardService);

  protected readonly vm = computed(() => {
    const d = this.dashboard.data();
    const sys = d?.system?.ok ? d.system.data : undefined;
    if (!d || !sys) return null;

    const temp = d.grafana?.ok ? d.grafana.data?.tempC : undefined;
    const since = new Date(Date.now() - sys.uptime * 1000);

    return {
      gauges: [
        { label: 'CPU', value: sys.cpu, text: `${sys.cpu.toFixed(0)}%`, lvl: level(sys.cpu) },
        { label: 'Memoria RAM', value: sys.ram, text: `${sys.ram.toFixed(0)}%`, lvl: level(sys.ram) },
        ...(temp != null
          ? [{ label: 'Temperatura', value: Math.min(temp, 100), text: `${temp.toFixed(0)}°`, lvl: level(temp, 60, 75) }]
          : []),
      ],
      uptime: uptime(sys.uptime),
      since: since.toLocaleString('es-ES', { dateStyle: 'medium', timeStyle: 'short' }),
      version: d.version,
      api: every(d.config.apiRefreshSeconds),
      weather: every(d.config.weatherRefreshSeconds),
      system: every(d.config.systemRefreshSeconds),
    };
  });
}

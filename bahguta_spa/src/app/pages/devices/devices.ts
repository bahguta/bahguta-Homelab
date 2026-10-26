import { Component, computed, inject } from '@angular/core';
import { DecimalPipe } from '@angular/common';
import { DashboardService } from '../../core/dashboard.service';

@Component({
  selector: 'app-devices',
  imports: [DecimalPipe],
  templateUrl: './devices.html',
  styleUrl: './devices.scss',
})
export class Devices {
  private readonly dashboard = inject(DashboardService);

  protected readonly counter = computed(() => this.dashboard.data()?.summary?.devices);

  /** Cada device (placa ESP32-C3) unido a su sensor (DHT22). */
  protected readonly items = computed(() => {
    const d = this.dashboard.data();
    const sensors = d?.sensors?.data ?? [];
    return (d?.devices?.data ?? []).map((dev) => ({
      dev,
      online: dev.status === 'ONLINE',
      s: sensors.find((x) => x.deviceId === dev.id),
    }));
  });

  protected ago(ts: number | null | undefined): string {
    if (ts == null) return 'Sin datos';
    const s = Math.max(0, Math.floor(Date.now() / 1000 - ts));
    if (s < 60) return 'hace unos segundos';
    const m = Math.floor(s / 60);
    if (m < 60) return `hace ${m} min`;
    const h = Math.floor(m / 60);
    return h < 24 ? `hace ${h} h` : `hace ${Math.floor(h / 24)} d`;
  }
}

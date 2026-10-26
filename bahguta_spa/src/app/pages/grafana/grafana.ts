import { Component, computed, inject } from '@angular/core';
import { DecimalPipe } from '@angular/common';
import { DashboardService } from '../../core/dashboard.service';
import { level } from '../../core/format';

@Component({
  selector: 'app-grafana',
  imports: [DecimalPipe],
  templateUrl: './grafana.html',
  styleUrl: './grafana.scss',
})
export class Grafana {
  private readonly dashboard = inject(DashboardService);

  protected readonly vm = computed(() => {
    const g = this.dashboard.data()?.grafana;
    if (!g?.ok || !g.data) return null;
    const x = g.data;

    const disk = (name: string, total: number, used: number, free: number) => {
      const pct = total > 0 ? (used / total) * 100 : 0;
      return { name, total, used, free, pct, lvl: level(pct) };
    };

    const disks = [disk('Sistema (/)', x.rootTotalGb, x.rootUsedGb, x.rootFreeGb)];
    if (x.storageTotalGb != null && x.storageUsedGb != null && x.storageFreeGb != null) {
      disks.push(disk('Almacenamiento (/srv/storage)', x.storageTotalGb, x.storageUsedGb, x.storageFreeGb));
    }

    return {
      disks,
      metrics: [
        { label: 'CPU', value: x.cpu, text: `${x.cpu.toFixed(0)}%`, lvl: level(x.cpu) },
        { label: 'Memoria', value: x.ram, text: `${x.ram.toFixed(0)}%`, lvl: level(x.ram) },
        { label: 'Temperatura', value: Math.min(x.tempC, 100), text: `${x.tempC.toFixed(0)} °C`, lvl: level(x.tempC, 60, 75) },
      ],
    };
  });
}

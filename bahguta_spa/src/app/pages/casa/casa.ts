import { Component, computed, effect, inject, signal, untracked } from '@angular/core';
import { DecimalPipe } from '@angular/common';
import { DashboardService } from '../../core/dashboard.service';
import { MeasurementsService } from '../../core/measurements.service';
import { RoomSeries } from '../../core/models';
import { ROOM_COLORS } from '../../core/room-colors';
import { RoomChart } from './room-chart';

@Component({
  selector: 'app-casa',
  imports: [DecimalPipe, RoomChart],
  templateUrl: './casa.html',
  styleUrl: './casa.scss',
})
export class Casa {
  private readonly dashboard = inject(DashboardService);
  private readonly measurements = inject(MeasurementsService);

  protected readonly metric = signal<'t' | 'h'>('t');
  protected readonly series = signal<RoomSeries[]>([]);
  protected readonly hours = signal(24);
  protected readonly historyError = signal(false);

  protected readonly rooms = computed(() => this.dashboard.data()?.rooms?.data ?? []);
  protected readonly summary = computed(() => this.dashboard.data()?.summary);
  protected readonly hasHistory = computed(() => this.series().some((r) => r.points.length > 0));

  /** Color por habitación, según el orden del dashboard. */
  protected readonly colors = computed(() =>
    Object.fromEntries(this.rooms().map((r, i) => [r.room, ROOM_COLORS[i % ROOM_COLORS.length]])),
  );

  constructor() {
    // Carga el histórico al entrar y cada vez que se actualiza el dashboard
    // (también con el botón de refrescar).
    effect(() => {
      this.dashboard.lastUpdate();
      untracked(() => this.loadHistory());
    });
  }

  private loadHistory(): void {
    this.measurements.summary().subscribe({
      next: (s) => {
        this.series.set(s.rooms);
        this.hours.set(Math.round((s.to - s.from) / 3600));
        this.historyError.set(false);
      },
      error: () => this.historyError.set(true),
    });
  }

  protected ago(ts: number | null): string {
    if (ts == null) return 'Sin datos';
    const s = Math.max(0, Math.floor(Date.now() / 1000 - ts));
    if (s < 60) return 'hace unos segundos';
    const m = Math.floor(s / 60);
    if (m < 60) return `hace ${m} min`;
    const h = Math.floor(m / 60);
    return h < 24 ? `hace ${h} h` : `hace ${Math.floor(h / 24)} d`;
  }
}

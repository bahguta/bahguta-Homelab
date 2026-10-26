import { Component, DestroyRef, ElementRef, effect, inject, input, viewChild } from '@angular/core';
import {
  CategoryScale,
  Chart,
  Legend,
  LineController,
  LineElement,
  LinearScale,
  PointElement,
  Tooltip,
} from 'chart.js';
import { RoomSeries } from '../../core/models';
import { ROOM_COLORS } from '../../core/room-colors';

Chart.register(CategoryScale, LinearScale, LineController, LineElement, PointElement, Legend, Tooltip);

const HOUR = new Intl.DateTimeFormat('es-ES', { hour: '2-digit', minute: '2-digit' });
const FULL = new Intl.DateTimeFormat('es-ES', {
  day: '2-digit',
  month: '2-digit',
  hour: '2-digit',
  minute: '2-digit',
});

/** Gráfico de líneas: una línea por habitación. */
@Component({
  selector: 'app-room-chart',
  template: `<div class="wrap"><canvas #canvas></canvas></div>`,
  styles: `
    :host { display: block; height: 100%; }
    .wrap { position: relative; width: 100%; height: 100%; }
  `,
})
export class RoomChart {
  readonly rooms = input.required<RoomSeries[]>();
  readonly metric = input<'t' | 'h'>('t');
  readonly colors = input<Record<string, string>>({});

  private readonly canvas = viewChild.required<ElementRef<HTMLCanvasElement>>('canvas');
  private chart?: Chart;
  private stamps: number[] = [];

  constructor() {
    inject(DestroyRef).onDestroy(() => this.chart?.destroy());

    effect(() => {
      const rooms = this.rooms();
      const metric = this.metric();
      const colors = this.colors();
      const unit = metric === 't' ? '°' : '%';

      // Eje X común: la unión de todas las marcas de tiempo.
      this.stamps = [...new Set(rooms.flatMap((r) => r.points.map((p) => p.ts)))].sort((a, b) => a - b);
      const labels = this.stamps.map((ts) => HOUR.format(ts * 1000));

      const datasets = rooms
        .filter((r) => r.points.length > 0)
        .map((r, i) => {
          const color = colors[r.room] ?? ROOM_COLORS[i % ROOM_COLORS.length];
          const byTs = new Map(r.points.map((p) => [p.ts, p[metric]]));
          return {
            label: r.room,
            data: this.stamps.map((ts) => byTs.get(ts) ?? null), // null = hueco en la línea
            borderColor: color,
            backgroundColor: color,
            borderWidth: 2,
            pointRadius: 2,
            pointHoverRadius: 5,
            tension: 0.3,
            spanGaps: false,
          };
        });

      if (this.chart) {
        this.chart.data.labels = labels;
        this.chart.data.datasets = datasets;
        this.chart.options.scales!['y']!.ticks!.callback = (v) => `${v}${unit}`;
        this.chart.update();
        return;
      }

      const css = getComputedStyle(document.documentElement);
      const muted = css.getPropertyValue('--muted').trim() || '#8b98a8';
      const border = css.getPropertyValue('--border').trim() || '#2a3441';

      this.chart = new Chart(this.canvas().nativeElement, {
        type: 'line',
        data: { labels, datasets },
        options: {
          responsive: true,
          maintainAspectRatio: false,
          interaction: { mode: 'index', intersect: false },
          scales: {
            x: { ticks: { color: muted, maxTicksLimit: 12 }, grid: { color: border } },
            y: { ticks: { color: muted, callback: (v) => `${v}${unit}` }, grid: { color: border } },
          },
          plugins: {
            legend: { labels: { color: muted, usePointStyle: true, boxWidth: 8 } },
            tooltip: {
              callbacks: {
                title: (items) => FULL.format(this.stamps[items[0].dataIndex] * 1000),
                label: (item) => `${item.dataset.label}: ${item.parsed.y}${metric === 't' ? '°' : '%'}`,
              },
            },
          },
        },
      });
    });
  }
}

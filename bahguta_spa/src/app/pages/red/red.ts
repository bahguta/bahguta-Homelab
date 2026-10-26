import { Component, computed, inject, signal } from '@angular/core';
import { DashboardService } from '../../core/dashboard.service';

const ipKey = (ip: string) => ip.split('.').reduce((a, n) => a * 256 + Number(n), 0);

@Component({
  selector: 'app-red',
  templateUrl: './red.html',
  styleUrl: './red.scss',
})
export class Red {
  private readonly dashboard = inject(DashboardService);

  protected readonly query = signal('');

  private readonly data = computed(() => {
    const r = this.dashboard.data()?.red;
    return r?.ok ? r.data : undefined;
  });

  protected readonly total = computed(() => this.data()?.total ?? 0);
  protected readonly available = computed(() => this.data() !== undefined);

  protected readonly clients = computed(() => {
    const q = this.query().trim().toLowerCase();
    return [...(this.data()?.clients ?? [])]
      .filter((c) => !q || [c.name, c.ip, c.mac].some((v) => v.toLowerCase().includes(q)))
      .sort((a, b) => ipKey(a.ip) - ipKey(b.ip));
  });

  protected icon(type: string): string {
    return type === 'wifi' ? '📶' : '🔌';
  }
}

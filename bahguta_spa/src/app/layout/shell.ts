import { Component, computed, inject } from '@angular/core';
import { DatePipe } from '@angular/common';
import { RouterLink, RouterLinkActive } from '@angular/router';
import { DashboardService } from '../core/dashboard.service';
import { InstallService } from '../core/install.service';

interface NavItem {
  path: string;
  label: string;
  icon: string;
}

@Component({
  selector: 'app-shell',
  imports: [RouterLink, RouterLinkActive, DatePipe],
  templateUrl: './shell.html',
  styleUrl: './shell.scss',
})
export class Shell {
  protected readonly dashboard = inject(DashboardService);
  protected readonly installer = inject(InstallService);

  /** Versiones para el footer: solo se muestran las que vienen en el JSON. */
  protected readonly versions = computed(() => {
    const d = this.dashboard.data();
    if (!d) return [];

    const v = (x: string) => (x.startsWith('v') ? x : `v${x}`);
    const out: { label: string; value: string }[] = [];

    if (d.version) out.push({ label: 'Back', value: v(d.version) });
    if (d.tabletVersion) out.push({ label: 'Tablet', value: v(d.tabletVersion) });

    // Sensores: versiones distintas de firmware entre los devices.
    const fws = [...new Set((d.devices?.data ?? []).map((x) => x.fw).filter(Boolean))];
    if (fws.length) out.push({ label: 'Sensores', value: fws.map(v).join(', ') });

    return out;
  });

  protected readonly nav: NavItem[] = [
    { path: '/home', label: 'Home', icon: '🏠' },
    { path: '/weather', label: 'Tiempo', icon: '⛅' },
    { path: '/casa', label: 'Casa', icon: '🛋️' },
    { path: '/docker', label: 'Docker', icon: '🐳' },
    { path: '/devices', label: 'Devices', icon: '📟' },
    { path: '/system', label: 'System', icon: '🖥️' },
    { path: '/red', label: 'Red', icon: '📶' },
    { path: '/grafana', label: 'Grafana', icon: '📊' },
  ];
}

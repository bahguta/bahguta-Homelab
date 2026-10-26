import { Component, computed, inject, signal } from '@angular/core';
import { HttpErrorResponse } from '@angular/common/http';
import { finalize } from 'rxjs';
import { DashboardService } from '../../core/dashboard.service';
import { DockerService } from '../../core/docker.service';
import { Container } from '../../core/models';

type Filter = 'all' | 'running' | 'stopped';

@Component({
  selector: 'app-docker',
  templateUrl: './docker.html',
  styleUrl: './docker.scss',
})
export class Docker {
  private readonly dashboard = inject(DashboardService);
  private readonly docker = inject(DockerService);

  protected readonly filter = signal<Filter>('all');
  protected readonly query = signal('');
  protected readonly busy = signal<string[]>([]);
  protected readonly error = signal<string | null>(null);

  private readonly all = computed(() => this.dashboard.data()?.docker?.data ?? []);

  protected readonly running = computed(() => this.all().filter((c) => c.state === 'running').length);
  protected readonly total = computed(() => this.all().length);

  protected readonly visible = computed(() => {
    const q = this.query().trim().toLowerCase();
    const f = this.filter();
    return this.all()
      .filter((c) => (f === 'all' ? true : f === 'running' ? c.state === 'running' : c.state !== 'running'))
      .filter((c) => !q || c.name.toLowerCase().includes(q))
      .sort((a, b) => Number(b.state === 'running') - Number(a.state === 'running') || a.name.localeCompare(b.name));
  });

  protected isBusy(id: string): boolean {
    return this.busy().includes(id);
  }

  protected toggle(c: Container): void {
    const isRunning = c.state === 'running';
    if (isRunning && !confirm(`¿Parar el contenedor "${c.name}"?`)) return;

    this.error.set(null);
    this.busy.update((b) => [...b, c.id]);

    this.docker
      .action(c.id, isRunning ? 'stop' : 'start')
      .pipe(finalize(() => this.busy.update((b) => b.filter((x) => x !== c.id))))
      .subscribe({
        next: () => this.dashboard.refresh(),
        error: (e: HttpErrorResponse) => this.error.set(this.message(e, c.name)),
      });
  }

  private message(e: HttpErrorResponse, name: string): string {
    if (e.status === 401) return 'No autorizado: usuario o contraseña incorrectos (o falta la X-API-Key en el proxy).';
    if (e.status === 403 || e.status === 500) return `No se pudo operar con "${name}": ¿está en la lista blanca?`;
    return `Error ${e.status || ''} al operar con "${name}".`;
  }
}

import { Injectable, inject, signal } from '@angular/core';
import { takeUntilDestroyed, toObservable } from '@angular/core/rxjs-interop';
import { HttpClient } from '@angular/common/http';
import { EMPTY, Subject, catchError, finalize, startWith, switchMap, timer } from 'rxjs';
import { Dashboard } from './models';

const API_URL = '/api/v1/dashboard/data';
const DEFAULT_POLL_MS = 30_000;

@Injectable({ providedIn: 'root' })
export class DashboardService {
  private readonly http = inject(HttpClient);
  private readonly pollMs = signal(DEFAULT_POLL_MS);
  private readonly refresh$ = new Subject<void>();

  readonly data = signal<Dashboard | null>(null);
  readonly error = signal<string | null>(null);
  readonly lastUpdate = signal<Date | null>(null);
  /** true mientras hay una petición en curso. */
  readonly loading = signal(false);

  constructor() {
    toObservable(this.pollMs)
      .pipe(
        // Al cambiar el intervalo, o al pedir una actualización manual,
        // se reinicia el temporizador y se consulta al momento.
        switchMap((ms) => this.refresh$.pipe(startWith(undefined), switchMap(() => timer(0, ms)))),
        switchMap(() => {
          this.loading.set(true);
          return this.http.get<Dashboard>(API_URL).pipe(
            catchError((e) => {
              this.error.set(e?.message ?? 'Error al consultar el backend');
              return EMPTY;
            }),
            finalize(() => this.loading.set(false)),
          );
        }),
        takeUntilDestroyed(),
      )
      .subscribe((d) => {
        this.data.set(d);
        this.error.set(null);
        this.lastUpdate.set(new Date());

        // Usa el mismo intervalo que el backend indica para la tablet.
        const ms = d.config?.apiRefreshSeconds * 1000;
        if (ms > 0 && ms !== this.pollMs()) this.pollMs.set(ms);
      });
  }

  /** Actualiza ahora, sin esperar al siguiente ciclo. */
  refresh(): void {
    this.refresh$.next();
  }
}

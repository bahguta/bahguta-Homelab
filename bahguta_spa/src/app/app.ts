import { Component, DestroyRef, inject } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { SwUpdate, VersionReadyEvent } from '@angular/service-worker';
import { filter } from 'rxjs';
import { Shell } from './layout/shell';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, Shell],
  template: `<app-shell><router-outlet /></app-shell>`,
})
export class App {
  constructor() {
    const sw = inject(SwUpdate);
    if (!sw.isEnabled) return; // en desarrollo (ng serve) el service worker está desactivado

    // Cuando hay una versión nueva ya descargada, se recarga para usarla.
    const sub = sw.versionUpdates
      .pipe(filter((e): e is VersionReadyEvent => e.type === 'VERSION_READY'))
      .subscribe(() => document.location.reload());

    // La ventana instalada puede quedarse abierta días: busca versiones nuevas cada 30 min.
    const timer = setInterval(() => sw.checkForUpdate().catch(() => {}), 30 * 60 * 1000);

    inject(DestroyRef).onDestroy(() => {
      sub.unsubscribe();
      clearInterval(timer);
    });
  }
}

import { Injectable, inject } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

const BASE = '/api/v1/system/docker/container';

/**
 * Arranca o para contenedores. La X-API-Key NO va aquí: la añade el proxy
 * (nginx en producción, proxy.conf.json en desarrollo).
 */
@Injectable({ providedIn: 'root' })
export class DockerService {
  private readonly http = inject(HttpClient);

  action(id: string, op: 'start' | 'stop'): Observable<unknown> {
    return this.http.post(`${BASE}/${encodeURIComponent(id)}/${op}`, null);
  }
}

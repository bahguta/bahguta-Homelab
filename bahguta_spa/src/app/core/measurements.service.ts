import { Injectable, inject } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable, map } from 'rxjs';
import { ApiResponse, MeasurementsSummary } from './models';

@Injectable({ providedIn: 'root' })
export class MeasurementsService {
  private readonly http = inject(HttpClient);

  /** Histórico por habitación (el back decide el rango; hoy 24 h cada hora). */
  summary(): Observable<MeasurementsSummary> {
    return this.http
      .get<ApiResponse<MeasurementsSummary>>('/api/v1/measurements/summary')
      .pipe(map((r) => r.data));
  }
}

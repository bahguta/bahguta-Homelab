import { Component, inject } from '@angular/core';
import { ActivatedRoute } from '@angular/router';

@Component({
  selector: 'app-placeholder',
  template: `
    <h1>{{ name }}</h1>
    <p class="muted">Pantalla pendiente de implementar.</p>
  `,
  styles: `.muted { color: var(--muted); }`,
})
export class Placeholder {
  protected readonly name: string = inject(ActivatedRoute).snapshot.data['name'] ?? '';
}

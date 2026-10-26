import { Routes } from '@angular/router';

export const routes: Routes = [
  { path: '', pathMatch: 'full', redirectTo: 'home' },
  {
    path: 'home',
    title: 'Home',
    loadComponent: () => import('./pages/home/home').then((m) => m.Home),
  },
  {
    path: 'casa',
    title: 'Casa',
    loadComponent: () => import('./pages/casa/casa').then((m) => m.Casa),
  },
  {
    path: 'weather',
    title: 'Tiempo',
    loadComponent: () => import('./pages/weather/weather').then((m) => m.Weather),
  },
  {
    path: 'docker',
    title: 'Docker',
    loadComponent: () => import('./pages/docker/docker').then((m) => m.Docker),
  },
  {
    path: 'devices',
    title: 'Devices',
    loadComponent: () => import('./pages/devices/devices').then((m) => m.Devices),
  },
  // Device y Sensor están unificados en una sola pantalla.
  { path: 'sensors', redirectTo: 'devices' },
  {
    path: 'system',
    title: 'System',
    loadComponent: () => import('./pages/system/system').then((m) => m.System),
  },
  {
    path: 'red',
    title: 'Red',
    loadComponent: () => import('./pages/red/red').then((m) => m.Red),
  },
  {
    path: 'grafana',
    title: 'Grafana',
    loadComponent: () => import('./pages/grafana/grafana').then((m) => m.Grafana),
  },
  { path: '**', redirectTo: 'home' },
];

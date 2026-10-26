import { Injectable, computed, signal } from '@angular/core';

/** Evento no estándar de Chrome/Edge/Android que permite lanzar el diálogo de instalación. */
interface BeforeInstallPromptEvent extends Event {
  prompt(): Promise<void>;
  userChoice: Promise<{ outcome: 'accepted' | 'dismissed' }>;
}

declare global {
  interface Window {
    /** Lo guarda el script de index.html: el evento puede llegar antes de que arranque Angular. */
    __bip?: BeforeInstallPromptEvent;
  }
}

const isStandalone = () =>
  window.matchMedia('(display-mode: standalone)').matches || (navigator as { standalone?: boolean }).standalone === true;

const isIos = () => /iphone|ipad|ipod/i.test(navigator.userAgent);

/** Instalación como aplicación (PWA): ventana propia, icono en el escritorio. */
@Injectable({ providedIn: 'root' })
export class InstallService {
  private deferred: BeforeInstallPromptEvent | null = null;

  private readonly installed = signal(isStandalone());
  private readonly promptReady = signal(false);

  /** Hay que enseñar el botón: se puede instalar y todavía no lo está. */
  readonly canInstall = computed(() => !this.installed() && (this.promptReady() || isIos()));

  constructor() {
    this.capture();
    window.addEventListener('bip-ready', () => this.capture());
    window.addEventListener('appinstalled', () => {
      this.deferred = null;
      this.promptReady.set(false);
      this.installed.set(true);
    });
  }

  private capture(): void {
    if (window.__bip) {
      this.deferred = window.__bip;
      this.promptReady.set(true);
    }
  }

  async install(): Promise<void> {
    if (this.deferred) {
      await this.deferred.prompt();
      await this.deferred.userChoice;
      this.deferred = null;
      window.__bip = undefined;
      this.promptReady.set(false);
    } else if (isIos()) {
      // Safari en iPhone/iPad no permite lanzar el diálogo: hay que hacerlo a mano.
      alert('Para instalarla: pulsa el botón Compartir y elige «Añadir a pantalla de inicio».');
    }
  }
}

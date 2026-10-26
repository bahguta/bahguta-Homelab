// Modelo del JSON de GET /api/v1/dashboard/data

export interface Section<T> {
  ok: boolean;
  updatedAt?: number;
  data?: T;
  error?: string;
}

// ---- Tiempo ----
export interface WeatherCurrent {
  location: string;
  temperature: number;
  feelsLike: number;
  minTemperature: number;
  maxTemperature: number;
  humidity: number;
  pressure: number;
  description: string;
  icon: string;
  windSpeed: number;
  windDirection: number;
  clouds: number;
  sunrise: number;
  sunset: number;
  timezone: number;
}

export interface ForecastHour {
  hour: number;
  min: number;
  max: number;
  icon: string;
}

export interface ForecastDay {
  date: string; // yyyy-MM-dd
  min: number;
  max: number;
  icon: string;
  description: string;
  rainMm: number;
  hours: ForecastHour[];
}

export interface WeatherData {
  current: WeatherCurrent;
  forecast: ForecastDay[];
}

// ---- Casa ----
export interface Room {
  room: string;
  temperature: number | null;
  humidity: number | null;
  lastMeasurement: number | null;
  stale: boolean;
}

export interface Counter {
  running: number | null;
  online: number | null;
  total: number;
}

export interface Summary {
  avgTemperature: number | null;
  avgHumidity: number | null;
  docker: Counter;
  devices: Counter;
  sensors: Counter;
}

// ---- Docker ----
export interface Container {
  id: string;
  name: string;
  state: string; // running | exited | ...
  status: string;
}

// ---- Devices / Sensors ----
export interface Device {
  id: string;
  name: string;
  description: string;
  hardwareModel: string;
  location: string;
  status: 'ONLINE' | 'OFFLINE' | string;
  deviceType: string;
  ip: string;
  mac: string;
  fw: string;
}

export interface Sensor {
  id: string;
  deviceId: string;
  name: string;
  type: string;
  enabled: boolean;
  interval: number;
  temperature: number | null;
  humidity: number | null;
  lastMeasurement: number | null;
  stale: boolean;
}

// ---- Sistema / Red / Grafana ----
export interface SystemData {
  cpu: number;
  ram: number;
  uptime: number; // segundos
}

export interface RedClient {
  name: string;
  ip: string;
  mac: string;
  type: string;
}

export interface RedData {
  total: number;
  clients: RedClient[];
}

export interface GrafanaData {
  rootTotalGb: number;
  rootUsedGb: number;
  rootFreeGb: number;
  storageTotalGb: number | null;
  storageUsedGb: number | null;
  storageFreeGb: number | null;
  cpu: number;
  ram: number;
  tempC: number;
}

// ---- Config ----
export interface AppConfig {
  apiRefreshSeconds: number;
  weatherRefreshSeconds: number;
  systemRefreshSeconds: number;
  mqttReconnectSeconds: number;
  deviceName: string;
  deviceLocation: string;
  brightness: number;
  screenAlwaysOn: boolean;
  screenTimeoutSeconds: number;
  forecastHour1: number;
  forecastHour2: number;
  forecastHour3: number;
}

// ---- Raíz ----
export interface Dashboard {
  success: boolean;
  ts: number;
  cfgVersion: number;
  weather: Section<WeatherData>;
  rooms: Section<Room[]>;
  summary: Summary;
  docker: Section<Container[]>;
  devices: Section<Device[]>;
  sensors: Section<Sensor[]>;
  system: Section<SystemData>;
  red: Section<RedData>;
  grafana: Section<GrafanaData>;
  config: AppConfig;
  /** Versión del backend. */
  version: string;
  /** Versión del firmware de la tablet: el backend aún no la envía. */
  tabletVersion?: string;
}

// ---- Histórico: GET /api/v1/measurements/summary ----
export interface ApiResponse<T> {
  success: boolean;
  timestamp: string;
  data: T;
  message: string | null;
}

export interface MeasurementPoint {
  ts: number; // epoch en segundos
  t: number | null; // temperatura
  h: number | null; // humedad
}

export interface RoomSeries {
  room: string;
  sensorId: string;
  points: MeasurementPoint[];
}

export interface MeasurementsSummary {
  from: number;
  to: number;
  stepSeconds: number;
  rooms: RoomSeries[];
}

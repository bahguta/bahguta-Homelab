-- Horas de referencia de las franjas del pronostico en WeatherScreen (hora local, 0-23).
-- Valores por defecto: 9:00, 14:00 y 19:00.
ALTER TABLE app_settings
    ADD COLUMN IF NOT EXISTS forecast_hour_1 integer NOT NULL DEFAULT 9  CHECK (forecast_hour_1 BETWEEN 0 AND 23),
    ADD COLUMN IF NOT EXISTS forecast_hour_2 integer NOT NULL DEFAULT 14 CHECK (forecast_hour_2 BETWEEN 0 AND 23),
    ADD COLUMN IF NOT EXISTS forecast_hour_3 integer NOT NULL DEFAULT 19 CHECK (forecast_hour_3 BETWEEN 0 AND 23);

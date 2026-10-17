FROM eclipse-temurin:21-jre
RUN apt-get update \
  && apt-get install -y --no-install-recommends docker.io \
  && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY bahguta-api.jar app.jar
EXPOSE 8080
ENTRYPOINT ["java","-jar","app.jar"]

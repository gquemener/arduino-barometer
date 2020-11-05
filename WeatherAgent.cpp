#include "WeatherAgent.h"

WeatherAgent::WeatherAgent(ApiClient *apiClient, Wifi *wifi, Barometer *barometer, Logger *logger)
{
  this->apiClient = apiClient;
  this->wifi = wifi;
  this->barometer = barometer;
  this->logger = logger;
}

void WeatherAgent::tick(unsigned long timestamp)
{
  if (this->firstTick) {
    while (!this->wifi->isConnected()) {
      this->wifi->connect();
    }

    this->apiClient->last24Hours(this->history);

    this->firstTick = false;
    return;
  }

  if (timestamp - this->lastMeasureTimestamp >= 15 * 60 * 1000) {
    this->logger->info("Measuring pressure...");
    Measure latestMeasure = this->barometer->measure();
    this->logger->info(String(latestMeasure.pressure()));

    for (unsigned int i = 0; i < 23; i++) {
      this->history[i] = this->history[i + 1];
    }
    this->history[23] = latestMeasure;
    this->pendingSync[this->currentPendingSyncIndex] = latestMeasure;
    this->currentPendingSyncIndex++;
    this->logger->info(String(String(this->currentPendingSyncIndex) + " measure(s) to push"));

    if (this->currentPendingSyncIndex >= 99) {
      this->currentPendingSyncIndex = 0;
      this->pendingBufferFull = true;
    }

    this->lastMeasureTimestamp = timestamp;
  }

  if (!this->wifi->isConnected()) {
    return;
  }

  if (this->pendingBufferFull) {
    // TODO: Sync every pending measures
    this->pendingBufferFull = false;
  }

  if (timestamp - this->lastPushTimestamp >= 1000 && this->currentPendingSyncIndex > 0) {
    this->logger->info(String("[" + String(this->currentPendingSyncIndex - 1) + "] Synchronizing..."));
    if (this->apiClient->pushMeasure(this->pendingSync[this->currentPendingSyncIndex - 1])) {
      this->currentPendingSyncIndex--;
      this->logger->info("done.");
    }
    this->lastPushTimestamp = timestamp;
  }
}

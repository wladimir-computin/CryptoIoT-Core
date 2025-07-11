/**
 * PowerMeter.h - Interface for power meter libraries
 *
 * A common interface for different power meter implementations,
 * based on the CSE7761 library structure.
 *
 * @author Generated by Claude
 * @date April 2025
 */

#pragma once

#include <Arduino.h>

/**
 * PowerMeter Interface
 * Common interface for various power meter implementations
 */
class PowerMeter {
public:

  virtual ~PowerMeter() {}
  /**
   * Initialize the power meter
   *
   * @return true if initialization was successful, false otherwise
   */
  virtual void setup() = 0;

  /**
   * Update measurements from the meter
   * Call this regularly to get fresh readings
   *
   * @return true if successful, false otherwise
   */
  virtual bool update() = 0;

  /**
   * Get the last measured voltage in volts
   *
   * @return Voltage in V
   */
  virtual float getVoltage() = 0;

  /**
   * Get the last measured current for a channel in amperes
   *
   * @param channel Channel number (typically 0 or 1)
   * @return Current in A
   */
  virtual float getCurrent(int channel) = 0;

  /**
   * Get the last measured active power for a channel in watts
   *
   * @param channel Channel number (typically 0 or 1)
   * @return Active power in W
   */
  virtual float getActivePower(uint8_t channel) = 0;

  /**
   * Check if the meter is ready
   *
   * @return true if ready, false otherwise
   */
  virtual bool isReady() = 0;
};

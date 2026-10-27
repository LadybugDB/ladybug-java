package com.ladybugdb;

/**
 * Runtime configuration for creating or opening a Database.
 *
 * <p>All fields default to {@code null}, which means "use the engine default".
 * Only explicitly set fields override the engine configuration, so engine-side
 * default changes apply automatically without a binding update.
 */
public class SystemConfig {
    public Long bufferPoolSize = null;
    public Long maxNumThreads = null;
    public Boolean enableCompression = null;
    public Boolean readOnly = null;
    public Long maxDBSize = null;
    public Boolean autoCheckpoint = null;
    public Long checkpointThreshold = null;
    public Boolean throwOnWalReplayFailure = null;
    public Boolean enableChecksums = null;
    public Boolean enableMultiWrites = null;
    public Boolean enableDefaultHashIndex = null;
}

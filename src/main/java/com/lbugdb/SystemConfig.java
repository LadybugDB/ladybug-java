package com.ladybugdb;

/**
 * Runtime configuration for creating or opening a Database.
 */
public class SystemConfig {
    public long bufferPoolSize = 0;
    public long maxNumThreads = 0;
    public boolean enableCompression = true;
    public boolean readOnly = false;
    public long maxDBSize = 0;
    public boolean autoCheckpoint = true;
    public long checkpointThreshold = -1;
    public boolean throwOnWalReplayFailure = true;
    public boolean enableChecksums = true;
    public boolean enableMultiWrites = false;
    public boolean enableDefaultHashIndex = true;
}

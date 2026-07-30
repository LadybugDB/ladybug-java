package com.ladybugdb;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.time.Duration;
import java.time.Instant;
import java.time.LocalDate;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.apache.arrow.c.ArrowSchema;
import org.apache.arrow.memory.BufferAllocator;
import org.apache.arrow.vector.VectorSchemaRoot;

/**
 * Connection is used to interact with a Database instance. Each Connection is thread-safe. Multiple
 * connections can connect to the same Database instance in a multi-threaded environment.
 */
public class Connection implements AutoCloseable {

    long conn_ref;
    boolean destroyed = false;

    /**
     * Creates a connection to the database.
     *
     * @param db: Database instance.
     */
    public Connection(Database db) {
        if (db == null)
            throw new AssertionError("Cannot create connection, database is null.");
        conn_ref = Native.lbugConnectionInit(db);
    }

    /**
     * Check if the connection has been destroyed.
     *
     * @throws RuntimeException If the connection has been destroyed.
     */
    private void checkNotDestroyed() {
        if (destroyed)
            throw new RuntimeException("Connection has been destroyed.");
    }

    /**
     * Close the connection and release the underlying resources. This method is invoked automatically on objects managed by the try-with-resources statement.
     *
     * @throws RuntimeException If the connection has been destroyed.
     */
    @Override
    public void close() {
        destroy();
    }

    /**
     * Destroy the connection.
     *
     * @throws RuntimeException If the connection has been destroyed.
     */
    private void destroy() {
        checkNotDestroyed();
        Native.lbugConnectionDestroy(this);
        destroyed = true;
    }

    /**
     * Return the maximum number of threads used for execution in the current connection.
     *
     * @return The maximum number of threads used for execution in the current connection.
     * @throws RuntimeException If the connection has been destroyed.
     */
    public long getMaxNumThreadForExec() {
        checkNotDestroyed();
        return Native.lbugConnectionGetMaxNumThreadForExec(this);
    }

    /**
     * Sets the maximum number of threads to use for execution in the current connection.
     *
     * @param numThreads: The maximum number of threads to use for execution in the current connection
     * @throws RuntimeException If the connection has been destroyed.
     */
    public void setMaxNumThreadForExec(long numThreads) {
        checkNotDestroyed();
        Native.lbugConnectionSetMaxNumThreadForExec(this, numThreads);
    }

    /**
     * Executes the given query and returns the result.
     *
     * @param queryStr: The query to execute.
     * @return The result of the query.
     * @throws RuntimeException If the connection has been destroyed.
     */
    public QueryResult query(String queryStr) {
        checkNotDestroyed();
        return Native.lbugConnectionQuery(this, queryStr);
    }

    /**
     * Prepares the given query and returns the prepared statement.
     *
     * @param queryStr: The query to prepare.
     * @return The prepared statement.
     * @throws RuntimeException If the connection has been destroyed.
     */
    public PreparedStatement prepare(String queryStr) {
        checkNotDestroyed();
        return Native.lbugConnectionPrepare(this, queryStr);
    }

    /**
     * Executes the given prepared statement with args and returns the result.
     *
     * <p>Values that are not already {@link Value} instances are automatically converted from
     * their boxed Java type (e.g. {@link String}, {@link Long}, {@link java.util.UUID}, etc.).
     * If a value's type is not supported, an {@link IllegalArgumentException} is thrown instead
     * of crashing the JVM.
     *
     * @param ps The prepared statement to execute.
     * @param params The parameter map. Each value must be a {@link Value} or one of the
     *               supported boxed types: Boolean, Byte, Short, Integer, Long, BigInteger,
     *               Float, Double, BigDecimal, String, InternalID, UUID, LocalDate, Instant,
     *               Duration.
     * @return The result of the query.
     * @throws RuntimeException         If the connection has been destroyed.
     * @throws IllegalArgumentException  If a parameter value has an unsupported type.
     */
    public QueryResult execute(PreparedStatement ps, Map<String, ?> params) {
        checkNotDestroyed();
        return Native.lbugConnectionExecute(this, ps, coerceParams(params));
    }

    /**
     * Convert the user-supplied {@code Map<String, ?>} into the strict
     * {@code Map<String, Value>} shape the JNI binding expects. Already-wrapped
     * {@link Value} instances are passed through (the JNI will clone them when
     * binding); boxed Java types are auto-converted via {@link Value#Value}.
     *
     * <p>Doing the conversion on the Java side keeps the public API ergonomic
     * ({@code Map<String, ?>} instead of forcing users into a raw-type cast)
     * while letting the native binding stay strictly typed — no
     * {@code @SuppressWarnings("unchecked")} required, and the conversion logic
     * is testable without round-tripping through JNI.
     */
    private static Map<String, Value> coerceParams(Map<String, ?> params) {
        Map<String, Value> coerced = new LinkedHashMap<>(params.size());
        for (Map.Entry<String, ?> e : params.entrySet()) {
            coerced.put(e.getKey(), coerceParam(e.getKey(), e.getValue()));
        }
        return coerced;
    }

    private static Value coerceParam(String key, Object v) {
        // Pattern-matching switch (JEP 441, GA in Java 21): the JIT lowers
        // this to a single type-table jump, so dispatch is O(1) per entry
        // regardless of how many supported types we add. Hot path for users
        // who bind a `Map<String, ?>` of mostly-uniform types — e.g. 1000
        // `Long` parameters — and previously paid 15 instanceof checks
        // per element.
        return switch (v) {
            case null ->
                // The JNI path used to surface null as IllegalArgumentException
                // ("unsupported type null"); preserve that contract. Users who
                // want a SQL NULL must build one explicitly via
                // {@link Value#createNull()}.
                throw new IllegalArgumentException(
                    "Parameter '" + key + "' is null; use Value.createNull() to bind SQL NULL.");
            case Value value -> value;
            case Boolean box -> new Value(box);
            case Byte box -> new Value(box);
            case Short box -> new Value(box);
            case Integer box -> new Value(box);
            case Long box -> new Value(box);
            case BigInteger box -> new Value(box);
            case Float box -> new Value(box);
            case Double box -> new Value(box);
            case BigDecimal box -> new Value(box);
            case String box -> new Value(box);
            case InternalID box -> new Value(box);
            case UUID box -> new Value(box);
            case LocalDate box -> new Value(box);
            case Instant box -> new Value(box);
            case Duration box -> new Value(box);
            default -> throw new IllegalArgumentException(
                "Parameter '" + key + "' has unsupported type " + v.getClass().getName()
                    + ". Accepted types: Value, Boolean, Byte, Short, Integer, Long, "
                    + "BigInteger, Float, Double, BigDecimal, String, InternalID, UUID, "
                    + "LocalDate, Instant, Duration");
        };
    }

    /**
     * Interrupts all queries currently executed within this connection.
     *
     * @throws RuntimeException If the connection has been destroyed.
     */
    public void interrupt() {
        checkNotDestroyed();
        Native.lbugConnectionInterrupt(this);
    }

    /**
     * Sets the query timeout value of the current connection. A value of zero (the default) disables the timeout.
     *
     * @param timeoutInMs: The query timeout value in milliseconds.
     * @throws RuntimeException If the connection has been destroyed.
     */
    public void setQueryTimeout(long timeoutInMs) {
        checkNotDestroyed();
        Native.lbugConnectionSetQueryTimeout(this, timeoutInMs);
    }

    /**
     * Registers Arrow memory as a node table. The first column is used as the table primary key.
     */
    public QueryResult createArrowTable(String tableName, List<VectorSchemaRoot> roots,
            BufferAllocator allocator) {
        checkNotDestroyed();
        try (ArrowSchema schema = ArrowSchema.allocateNew(allocator);
                ArrowArrays arrays = ArrowUtil.exportRoots(allocator, roots, schema)) {
            return Native.lbugConnectionCreateArrowTable(this, tableName, schema.memoryAddress(),
                    arrays.address(), roots.size());
        }
    }

    /**
     * Registers Arrow memory as a relationship table with endpoint columns named "from" and "to".
     */
    public QueryResult createArrowRelTable(String tableName, List<VectorSchemaRoot> roots,
            String srcTableName, String dstTableName, BufferAllocator allocator) {
        checkNotDestroyed();
        try (ArrowSchema schema = ArrowSchema.allocateNew(allocator);
                ArrowArrays arrays = ArrowUtil.exportRoots(allocator, roots, schema)) {
            return Native.lbugConnectionCreateArrowRelTable(this, tableName, srcTableName,
                    dstTableName, schema.memoryAddress(), arrays.address(), roots.size());
        }
    }

    /**
     * Registers Arrow memory in CSR form as a relationship table.
     */
    public QueryResult createArrowRelTableCSR(String tableName, List<VectorSchemaRoot> indicesRoots,
            List<VectorSchemaRoot> indptrRoots, String srcTableName, String dstTableName,
            String dstColumnName, BufferAllocator allocator) {
        checkNotDestroyed();
        try (ArrowSchema indicesSchema = ArrowSchema.allocateNew(allocator);
                ArrowArrays indicesArrays = ArrowUtil.exportRoots(allocator, indicesRoots,
                        indicesSchema);
                ArrowSchema indptrSchema = ArrowSchema.allocateNew(allocator);
                ArrowArrays indptrArrays = ArrowUtil.exportRoots(allocator, indptrRoots,
                        indptrSchema)) {
            return Native.lbugConnectionCreateArrowRelTableCSR(this, tableName, srcTableName,
                    dstTableName, indicesSchema.memoryAddress(), indicesArrays.address(),
                    indicesRoots.size(), indptrSchema.memoryAddress(), indptrArrays.address(),
                    indptrRoots.size(), dstColumnName);
        }
    }

    /**
     * Drops an Arrow memory-backed table registered on this connection.
     */
    public QueryResult dropArrowTable(String tableName) {
        checkNotDestroyed();
        return Native.lbugConnectionDropArrowTable(this, tableName);
    }
}

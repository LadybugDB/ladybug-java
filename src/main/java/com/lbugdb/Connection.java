package com.ladybugdb;

import java.util.Map;
import java.util.List;

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
     * @param ps: The prepared statement to execute.
     * @param m:  The parameter pack where each arg is a std::pair with the first element being parameter name and second
     *            element being parameter value
     * @return The result of the query.
     * @throws RuntimeException If the connection has been destroyed.
     */
    public QueryResult execute(PreparedStatement ps, Map<String, Value> m) {
        checkNotDestroyed();
        return Native.lbugConnectionExecute(this, ps, m);
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

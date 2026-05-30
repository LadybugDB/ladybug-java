package com.ladybugdb;

import java.util.List;

import org.apache.arrow.c.ArrowArray;
import org.apache.arrow.c.ArrowSchema;
import org.apache.arrow.c.CDataDictionaryProvider;
import org.apache.arrow.c.Data;
import org.apache.arrow.memory.BufferAllocator;
import org.apache.arrow.vector.VectorSchemaRoot;

final class ArrowUtil {
    private ArrowUtil() {
    }

    static ArrowArrays exportRoots(BufferAllocator allocator, List<VectorSchemaRoot> roots,
            ArrowSchema schema) {
        if (roots == null || roots.isEmpty()) {
            throw new IllegalArgumentException("At least one Arrow VectorSchemaRoot is required.");
        }
        ArrowArrays arrays = new ArrowArrays(roots.size());
        boolean success = false;
        try {
            for (int i = 0; i < roots.size(); i++) {
                VectorSchemaRoot root = roots.get(i);
                if (!root.getSchema().equals(roots.get(0).getSchema())) {
                    throw new IllegalArgumentException("All Arrow VectorSchemaRoot schemas must match.");
                }
                ArrowArray array = arrays.wrap(i);
                if (i == 0) {
                    Data.exportVectorSchemaRoot(allocator, root, null, array, schema);
                } else {
                    Data.exportVectorSchemaRoot(allocator, root, null, array);
                }
            }
            success = true;
            return arrays;
        } finally {
            if (!success) {
                arrays.close();
            }
        }
    }

    static VectorSchemaRoot importRoot(BufferAllocator allocator, ArrowSchema schema,
            ArrowArray array) {
        return Data.importVectorSchemaRoot(allocator, array, schema,
                (CDataDictionaryProvider) null);
    }
}

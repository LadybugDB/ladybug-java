package com.ladybugdb;

import java.util.ArrayList;
import java.util.List;

import org.apache.arrow.c.ArrowArray;

final class ArrowArrays implements AutoCloseable {
    private long address;
    private final List<ArrowArray> wrappers = new ArrayList<>();

    ArrowArrays(long numArrays) {
        address = Native.lbugArrowArrayAllocate(numArrays);
    }

    long address() {
        return address;
    }

    long elementAddress(long index) {
        return Native.lbugArrowArrayGetAddress(address, index);
    }

    ArrowArray wrap(long index) {
        ArrowArray array = ArrowArray.wrap(elementAddress(index));
        wrappers.add(array);
        return array;
    }

    @Override
    public void close() {
        for (ArrowArray wrapper : wrappers) {
            wrapper.close();
        }
        wrappers.clear();
        if (address != 0) {
            Native.lbugArrowArrayFree(address);
            address = 0;
        }
    }
}

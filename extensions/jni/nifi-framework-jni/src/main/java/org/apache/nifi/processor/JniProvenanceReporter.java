package org.apache.nifi.processor;

import org.apache.nifi.flowfile.FlowFile;
import org.apache.nifi.provenance.ProvenanceReporter;

import java.util.Collection;

public class JniProvenanceReporter implements ProvenanceReporter {


    private long nativePtr;


    @Override
    public void receive(FlowFile flowFile, String transitUri) {
        receive(flowFile,transitUri,"","", -1);
    }

    @Override
    public void receive(FlowFile flowFile, String transitUri, String sourceSystemFlowFileIdentifier) {
        receive(flowFile,transitUri,"",sourceSystemFlowFileIdentifier, -1);
    }

    @Override
    public void receive(FlowFile flowFile, String transitUri, long transmissionMillis) {
        receive(flowFile,transitUri,"","", transmissionMillis);
    }

    @Override
    public void receive(FlowFile flowFile, String transitUri, String details, long transmissionMillis) {
        receive(flowFile,transitUri,"",details, transmissionMillis);
    }

    @Override
    public native void receive(FlowFile flowFile, String transitUri, String sourceSystemFlowFileIdentifier, String details, long transmissionMillis);

    @Override
    public void fetch(FlowFile flowFile, String transitUri) {
        fetch(flowFile,transitUri,"",-1);
    }

    @Override
    public void fetch(FlowFile flowFile, String transitUri, long transmissionMillis) {
        fetch(flowFile,transitUri,"",transmissionMillis);
    }

    @Override
    public native void fetch(FlowFile flowFile, String transitUri, String details, long transmissionMillis);

    @Override
    public void send(FlowFile flowFile, String transitUri) {
        send(flowFile,transitUri,"",-1,false);
    }

    @Override
    public void send(FlowFile flowFile, String transitUri, String details) {
        send(flowFile,transitUri,details,-1,false);
    }

    @Override
    public void send(FlowFile flowFile, String transitUri, long transmissionMillis) {
        send(flowFile,transitUri,"",transmissionMillis,false);
    }

    @Override
    public void send(FlowFile flowFile, String transitUri, String details, long transmissionMillis) {
        send(flowFile,transitUri,details,transmissionMillis,false);
    }

    @Override
    public void send(FlowFile flowFile, String transitUri, boolean force) {
        send(flowFile,transitUri,"",-1,force);
    }

    @Override
    public void send(FlowFile flowFile, String transitUri, String details, boolean force) {
        send(flowFile,transitUri,details,-1,force);
    }

    @Override
    public void send(FlowFile flowFile, String transitUri, long transmissionMillis, boolean force) {
        send(flowFile,transitUri,"",transmissionMillis,force);
    }

    @Override
    public native void send(FlowFile flowFile, String transitUri, String details, long transmissionMillis, boolean force);

    @Override
    public void invokeRemoteProcess(FlowFile flowFile, String transitUri) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void invokeRemoteProcess(FlowFile flowFile, String transitUri, String details) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void associate(FlowFile flowFile, String alternateIdentifierNamespace, String alternateIdentifier) {
        throw new UnsupportedOperationException();
    }

    @Override
    public void fork(FlowFile parent, Collection<FlowFile> children) {
        fork(parent,children,"",-1);
    }

    @Override
    public void fork(FlowFile parent, Collection<FlowFile> children, String details) {
        fork(parent,children,details,-1);
    }

    @Override
    public void fork(FlowFile parent, Collection<FlowFile> children, long forkDuration) {
        fork(parent,children,"",forkDuration);
    }

    @Override
    public native void fork(FlowFile parent, Collection<FlowFile> children, String details, long forkDuration);

    @Override
    public void join(Collection<FlowFile> parents, FlowFile child) {
        join(parents,child,"",-1);
    }

    @Override
    public void join(Collection<FlowFile> parents, FlowFile child, String details) {
        join(parents,child,details,-1);
    }

    @Override
    public void join(Collection<FlowFile> parents, FlowFile child, long joinDuration) {
        join(parents,child,"",joinDuration);
    }

    @Override
    public native void join(Collection<FlowFile> parents, FlowFile child, String details, long joinDuration);

    @Override
    public native void clone(FlowFile parent, FlowFile child);

    @Override
    public void modifyContent(FlowFile flowFile) {
        modifyContent(flowFile,"",-1);
    }

    @Override
    public void modifyContent(FlowFile flowFile, String details) {
        modifyContent(flowFile,details,-1);
    }

    @Override
    public void modifyContent(FlowFile flowFile, long processingMillis) {
        modifyContent(flowFile,"",processingMillis);
    }

    @Override
    public native void modifyContent(FlowFile flowFile, String details, long processingMillis) ;

    @Override
    public void modifyAttributes(FlowFile flowFile) {
        modifyAttributes(flowFile,"");
    }

    @Override
    public native void modifyAttributes(FlowFile flowFile, String details);

    @Override
    public void route(FlowFile flowFile, Relationship relationship) {
        route(flowFile,relationship,"",-1);
    }

    @Override
    public void route(FlowFile flowFile, Relationship relationship, String details) {
        route(flowFile,relationship,details,-1);
    }

    @Override
    public void route(FlowFile flowFile, Relationship relationship, long processingDuration) {
        route(flowFile,relationship,"",processingDuration);
    }

    @Override
    public native void route(FlowFile flowFile, Relationship relationship, String details, long processingDuration);

    @Override
    public void create(FlowFile flowFile) {
        create(flowFile,"");
    }

    @Override
    public native void create(FlowFile flowFile, String details);
}

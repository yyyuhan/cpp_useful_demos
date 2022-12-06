#include <iostream>

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "cmake/build/streaming.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using grpc::Status;

using streampack::Communicator;
using streampack::NormRequest;
using streampack::NormResponse;

using std::cout;
using std::endl;

class NormService final : public Communicator::Service
{
    grpc::Status UnaryComm(::grpc::ServerContext *context, const NormRequest *request, NormResponse *response)
    {
        cout << "[UnaryComm] server got request with id: " << request->id() << endl;
        response->set_ret(0);
        response->set_msg("ok");
        return Status::OK;
    }

    grpc::Status ClientStreamComm(::grpc::ServerContext *context, ::grpc::ServerReader< ::streampack::NormRequest> *reader, ::streampack::NormResponse *response)
    {
        NormRequest request;
        while (reader->Read(&request))
        {
            cout << "[ClientStreamComm] server got request with id: " << request.id() << endl;
        }
        response->set_ret(0);
        response->set_msg("ok, received all streamed requests");
        return Status::OK;
    }
};

void RunServer()
{
    std::string server_address("0.0.0.0:50051");
    NormService service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main()
{
    RunServer();

    return 0;
}
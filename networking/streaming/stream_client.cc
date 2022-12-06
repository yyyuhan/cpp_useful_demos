#include <iostream>

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include "cmake/build/streaming.grpc.pb.h"
#include "utils/constants.h"

using std::cout;
using std::endl;
using std::string;

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using streampack::Communicator;
using streampack::NormRequest;
using streampack::NormResponse;

// max msg len: 4194304
static const int CHUNK_SIZE = 1024 * 1024 * 2;

static inline void
CreateNormRequest(NormRequest *req)
{
    static int s_id = 0;
    req->set_id(s_id++);
    req->set_content(string(CHUNK_SIZE, '*'));
}

class CommClient
{
public:
    CommClient(std::shared_ptr<grpc::Channel> channel) : stub_(Communicator::NewStub(channel))
    {
    }

    Status UnarySendRecv(const NormRequest &req, NormResponse *resp)
    {
        ClientContext context;
        return stub_->UnaryComm(&context, req, resp);
    }

    Status ClientStreamSendRecv(const NormRequest &req, NormResponse *resp)
    {
        ClientContext context;
        auto writer = stub_->ClientStreamComm(&context, resp);

        NormRequest mut_req = req;
        auto options = grpc::WriteOptions();
        for (int i = 0; i < 5; ++i)
        {

            mut_req.set_id(i + 1);
            writer->Write(mut_req, options);
        }
        writer->WritesDone();
        return writer->Finish();
    }

    template <class RespT>
    void EvalStatus(Status status, const RespT &resp)
    {
        if (status.ok())
        {
            cout << "client got a success status!" << endl;
            cout << resp.DebugString() << endl;
        }
        else
        {
            cout << "client got a failure status. err: " << status.error_message() << "\n"
                 << status.error_details() << endl;
        }
    }

private:
    std::unique_ptr<Communicator::Stub> stub_;
};

int main()
{
    // create channel
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());

    CommClient client = CommClient(channel);

    cout << PRINT_DELIM(unary send recv) << endl;
    NormRequest req;
    NormResponse resp;
    CreateNormRequest(&req);
    auto status = client.UnarySendRecv(req, &resp);
    client.EvalStatus(status, resp);

    status = client.ClientStreamSendRecv(req, &resp);
    client.EvalStatus(status, resp);
}
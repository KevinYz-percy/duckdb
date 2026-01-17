#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "database.grpc.pb.h"

using duckdb_grpc::DatabaseService;
using duckdb_grpc::QueryRequest;
using duckdb_grpc::QueryResponse;
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

class DatabaseClient {
public:
	DatabaseClient(std::shared_ptr<Channel> channel) : stub_(DatabaseService::NewStub(channel)) {
	}

	void ExecuteQuery(const std::string &sql) {
		QueryRequest request;
		request.set_sql(sql);
		QueryResponse reply;
		ClientContext context;

		Status status = stub_->ExecuteQuery(&context, request, &reply);

		if (status.ok()) {
			if (reply.success()) {
				std::cout << "Query successful!" << std::endl;
				for (const auto &row : reply.rows()) {
					for (const auto &val : row.values()) {
						std::cout << val << " | ";
					}
					std::cout << std::endl;
				}
			} else {
				std::cout << "Query failed: " << reply.error_message() << std::endl;
			}
		} else {
			std::cout << "RPC failed" << std::endl;
		}
	}

private:
	std::unique_ptr<DatabaseService::Stub> stub_;
};

int main(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "Usage: ./duckdb_client \"SELECT ...\"" << std::endl;
		return 1;
	}

	std::string target_str = "localhost:50051";
	DatabaseClient client(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

	std::string sql = argv[1];
	client.ExecuteQuery(sql);

	return 0;
}

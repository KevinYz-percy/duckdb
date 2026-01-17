#include <iostream>
#include <string>
#include <memory>

#include <grpcpp/grpcpp.h>
#include <duckdb.hpp>

#include "database.grpc.pb.h"

using duckdb_grpc::DatabaseService;
using duckdb_grpc::QueryRequest;
using duckdb_grpc::QueryResponse;
using duckdb_grpc::Row;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

// Logic will go here
class DatabaseServiceImpl final : public DatabaseService::Service {
	duckdb::DuckDB db_;
	duckdb::Connection conn_;

public:
	DatabaseServiceImpl() : db_(nullptr), conn_(db_) {
		// Initialize simple in-memory DB or persistent
	}

	// ctor adjustment
	DatabaseServiceImpl(const char *path) : db_(path), conn_(db_) {
	}

	Status ExecuteQuery(ServerContext *context, const QueryRequest *request, QueryResponse *reply) override {
		std::cout << "Executing query: " << request->sql() << std::endl;

		try {
			// Execute SQL using DuckDB
			auto result = conn_.Query(request->sql());

			// Check for errors
			if (result->HasError()) {
				reply->set_success(false);
				reply->set_error_message(result->GetError());
				return Status::OK;
			}

			reply->set_success(true);

			// Iterate result set and populate response
			for (duckdb::idx_t i = 0; i < result->RowCount(); i++) {
				Row *row = reply->add_rows();
				for (duckdb::idx_t j = 0; j < result->ColumnCount(); j++) {
					// Convert all values to string for simplicity
					row->add_values(result->GetValue(j, i).ToString());
				}
			}

		} catch (std::exception &e) {
			reply->set_success(false);
			reply->set_error_message(e.what());
		}

		return Status::OK;
	}
};

int main() {
	std::string server_address("0.0.0.0:50051");
	// In-memory DB
	DatabaseServiceImpl service(nullptr);

	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
	return 0;
}

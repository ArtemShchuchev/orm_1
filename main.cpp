#include <iostream>
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>

#include "SecondaryFunction.h"

class Publisher;
class Book;
class Shop;
class Stock;
class Sale;
void createrecords(Wt::Dbo::Session&);
void takerecords(Wt::Dbo::Session&);

int main(int argc, char** argv)
{
	printHeader(L"Домашнее задание к занятию «C++ и БД. ORM»");

	const std::string connectionstring(
		"host=localhost "
		"port=5432 "
		"dbname=WorkPostgre "
		"user=postgres "
		"password=postgres");

	try
	{
		auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(connectionstring);
		Wt::Dbo::Session session;
		session.setConnection(std::move(postgres));

		session.mapClass<Publisher>("Publisher");
		session.mapClass<Book>("Book");
		session.mapClass<Shop>("Shop");
		session.mapClass<Stock>("Stock");
		session.mapClass<Sale>("Sale");

		try
		{
			session.createTables();
			createrecords(session);
		}
		catch (const std::exception& err)
		{
			consoleCol(col::br_red);
			std::wcout << L"\nОшибка типа: " << typeid(err).name() << "\n";
			std::wcerr << L"\nСообщение об ошибке:\n";
			std::wcerr << utf2wide(err.what()) << '\n';
			consoleCol(col::cancel);
		}

		takerecords(session);
	}
	catch (std::exception const& err)
	{
		consoleCol(col::br_red);
		std::wcout << L"\nОшибка типа: " << typeid(err).name() << "\n";
		std::wcerr << L"\nСообщение об ошибке:\n";
		std::wcerr << utf2wide(err.what()) << '\n';
		consoleCol(col::cancel);
	}

	std::wcout << "\n";

	return 0;
}

// издатель
class Publisher
{
public:
	std::string name;
	Wt::Dbo::collection<Wt::Dbo::ptr<Book>> book;

	Publisher(){}
	Publisher(const std::string& _name) : name(_name) {}

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, book, Wt::Dbo::ManyToOne, "publisher");
	}
};
//книга
class Book
{
public:
	std::string title;
	Wt::Dbo::ptr<Publisher> publisher;
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stock;

	Book() {}
	Book(const std::string& _title, Wt::Dbo::ptr<Publisher> pub) : title(_title), publisher(pub) {}

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, title, "title");
		Wt::Dbo::belongsTo(a, publisher, "publisher");
		Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "book");
	}
};
// магазин
class Shop
{
public:
	std::string name;
	Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stock;

	Shop() {}
	Shop(const std::string& _name) : name(_name) {}

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::hasMany(a, stock, Wt::Dbo::ManyToOne, "shop");
	}
};
//стопка
class Stock
{
public:
	int count;
	Wt::Dbo::ptr<Shop> shop;
	Wt::Dbo::ptr<Book> book;
	Wt::Dbo::collection<Wt::Dbo::ptr<Sale>> sale;

	Stock() : count(0) {}
	// стопка в магазине "sp", содержит книги "bk" в количестве "amount"
	Stock(Wt::Dbo::ptr<Shop> sp, Wt::Dbo::ptr<Book> bk, const int amount) : shop(sp), book(bk), count(amount) {}

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, shop, "shop");
		Wt::Dbo::belongsTo(a, book, "book");
		Wt::Dbo::hasMany(a, sale, Wt::Dbo::ManyToOne, "stock");
	}
};
//продажи
class Sale
{
public:
	double price;
	std::string date_sale;
	int count;
	Wt::Dbo::ptr<Stock> stock;

	Sale() : price(0.0), count(0) {}
	// продано по цене "cost", дата "date" в количестве "amount" из стопки "st"
	Sale(const double cost, const std::string& date, const int amount, Wt::Dbo::ptr<Stock> st)
		: price(cost), date_sale(date), count(amount), stock(st) {}

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, price, "price");
		Wt::Dbo::field(a, date_sale, "date_sale");
		Wt::Dbo::field(a, count, "count");
		Wt::Dbo::belongsTo(a, stock, "stock");
	}
};

//создаю записи
void createrecords(Wt::Dbo::Session& s)
{
	Wt::Dbo::Transaction transaction{ s };

	auto Gorky_ptr = s.add(std::make_unique<Publisher>("Maksim Gorky"));
	auto mg1 = s.add(std::make_unique<Book>("Detstvo", Gorky_ptr));
	auto mg2 = s.add(std::make_unique<Book>("Na dne", Gorky_ptr));
	auto mg3 = s.add(std::make_unique<Book>("Chelkash", Gorky_ptr));
	auto mg4 = s.add(std::make_unique<Book>("Troe", Gorky_ptr));
	auto mg5 = s.add(std::make_unique<Book>("Dachniki", Gorky_ptr));

	auto Lev_ptr = s.add(std::make_unique<Publisher>("Lev Tolstoj"));
	auto lt1 = s.add(std::make_unique<Book>("Vojna i mir", Lev_ptr));
	auto lt2 = s.add(std::make_unique<Book>("Anna Karenina", Lev_ptr));
	auto lt3 = s.add(std::make_unique<Book>("Otrochestvo", Lev_ptr));
	auto lt4 = s.add(std::make_unique<Book>("Ispoved", Lev_ptr));
	auto lt5 = s.add(std::make_unique<Book>("Kazaki", Lev_ptr));

	auto Gogol_ptr = s.add(std::make_unique<Publisher>("Nikolay Gogol"));
	auto ng1 = s.add(std::make_unique<Book>("Vij", Gogol_ptr));
	auto ng2 = s.add(std::make_unique<Book>("Mertvye dushi", Gogol_ptr));
	auto ng3 = s.add(std::make_unique<Book>("Taras Bulba", Gogol_ptr));
	auto ng4 = s.add(std::make_unique<Book>("Revizor", Gogol_ptr));
	auto ng5 = s.add(std::make_unique<Book>("Portret", Gogol_ptr));

	auto Chehov_ptr = s.add(std::make_unique<Publisher>("Anton Chehov"));
	auto ac1 = s.add(std::make_unique<Book>("Vishnevyj sad", Chehov_ptr));
	auto ac2 = s.add(std::make_unique<Book>("Tolstyj i tonkij", Chehov_ptr));
	auto ac3 = s.add(std::make_unique<Book>("Toska", Chehov_ptr));
	auto ac4 = s.add(std::make_unique<Book>("Kashtanka", Chehov_ptr));

	auto shop1 = s.add(std::make_unique<Shop>("Biblioteka"));
	auto shop2 = s.add(std::make_unique<Shop>("Na Kamenke"));

	auto stock01 = s.add(std::make_unique<Stock>(shop1, mg1, 20));
	auto stock02 = s.add(std::make_unique<Stock>(shop1, mg2, 12));
	auto stock03 = s.add(std::make_unique<Stock>(shop1, mg3, 14));
	auto stock04 = s.add(std::make_unique<Stock>(shop1, mg4, 6));
	auto stock05 = s.add(std::make_unique<Stock>(shop1, mg5, 6));
	auto stock06 = s.add(std::make_unique<Stock>(shop1, lt1, 16));
	auto stock07 = s.add(std::make_unique<Stock>(shop1, lt2, 26));
	auto stock08 = s.add(std::make_unique<Stock>(shop1, lt3, 4));
	auto stock09 = s.add(std::make_unique<Stock>(shop1, lt4, 31));
	auto stock10 = s.add(std::make_unique<Stock>(shop1, lt5, 16));
	auto stock11 = s.add(std::make_unique<Stock>(shop2, ng1, 5));
	auto stock12 = s.add(std::make_unique<Stock>(shop2, ng2, 15));
	auto stock13 = s.add(std::make_unique<Stock>(shop2, ng3, 22));
	auto stock14 = s.add(std::make_unique<Stock>(shop2, ng4, 12));
	auto stock15 = s.add(std::make_unique<Stock>(shop2, ng5, 13));
	auto stock16 = s.add(std::make_unique<Stock>(shop2, ac1, 5));
	auto stock17 = s.add(std::make_unique<Stock>(shop2, ac2, 17));
	auto stock18 = s.add(std::make_unique<Stock>(shop2, ac3, 11));
	auto stock19 = s.add(std::make_unique<Stock>(shop2, ac4, 14));

	auto sale1 = s.add(std::make_unique<Sale>(536.5, "21.02.2023", 2, stock01));
	auto sale2 = s.add(std::make_unique<Sale>(640.3, "12.03.2023", 1, stock11));
	auto sale3 = s.add(std::make_unique<Sale>(380.0, "03.03.2023", 3, stock09));
	auto sale4 = s.add(std::make_unique<Sale>(700.0, "02.01.2023", 4, stock17));
	auto sale5 = s.add(std::make_unique<Sale>(536.3, "14.02.2023", 2, stock05));

	transaction.commit();
}

void takerecords(Wt::Dbo::Session& s)
{
	Wt::Dbo::Transaction transaction{ s };
	// коллекция издателей
	Wt::Dbo::collection<Wt::Dbo::ptr<Publisher>> publisher = s.find<Publisher>();
	std::wcout << L"Библиотека содержит " << publisher.size() << L" издателя:\n";
	for (const auto& p : publisher)
	{
		// показываю всех известных базе издателей
		std::wcout << p.id() << ") " << utf2wide(p->name) << "\n";
	}
	std::wcout << L"Введи id издателя для поиска по базе магазинов: ";
	int idpub(0);
	std::wcin >> idpub;

	// Писатель выбранный пользователем
	Wt::Dbo::ptr<Publisher> userpub = s.find<Publisher>().where("id = ?").bind(idpub);
	std::wcout << L"\nПисателя: " << utf2wide(userpub->name) << L" можно найти в следующих магазинах:\n";
	
	// книги написанные, выбранным писателем
	Wt::Dbo::collection<Wt::Dbo::ptr<Book>> book = (userpub->book).find().where("publisher_id=?").bind(idpub);
	for (const auto& b : book)
	{
		// стопки в которых хранятся книги, выбранного писателя
		Wt::Dbo::collection<Wt::Dbo::ptr<Stock>> stock = b->stock.find().where("book_id=?").bind(b.id());
		for (const auto& st : stock)
		{
			std::wcout << L"В стопке: " << st.id() << L", магазин: " << utf2wide((st->shop)->name) << "\n";
		}
	}

	transaction.commit();
}
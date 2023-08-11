
#include"stdh.hpp"

auto main()->int
{
	using identifier = size_t;
	using rank = size_t;

	std::wifstream ifs;
	std::wofstream ofs;
	std::vector<std::wstring> names{};
	std::vector<identifier> identifiers{};
	std::vector<rank> ranks{};
	std::unordered_map<identifier, rank> drawing_basket{};
	std::multimap<rank, std::wstring, std::greater<rank>> ordered_ranks{};

	try
	{
		ifs.open("participants.txt", std::ios::in);
		ofs.open("ledger.txt", std::ios::out);

		// participants names reading from file
		names.reserve(100Ui64);
		std::wstring name{};
		while (!ifs.eof())
		{
			std::getline(ifs, name);
			names.emplace_back(name);
		}
		ifs.close();
		
		// identifiers preparation
		identifiers.reserve(names.size());
		auto id{ 0Ui64 };
		for (const auto& e : names)
			identifiers.emplace_back(id++);
		
		do
		{
			// drawing procedure
			std::random_device rd{};
			std::mt19937_64 rng{ rd() };
			std::uniform_int_distribution<identifier> rnd(0Ui64, identifiers.size() - 1Ui64);
			ranks.resize(identifiers.size() * 1000Ui64, 0Ui64);
			std::ranges::generate(
				ranks,
				[&rng, &rnd]()
				{
					return rnd(rng);
				}
			);
			std::ranges::for_each(
				ranks,
				[&drawing_basket](const rank& r)
				{
					drawing_basket[r]++;
				}
			);

			// temporary display
			ofs 
				<< L"\n-------------------------------------------\n" 
				<< L"\tdrawing basket:"
				<< L"\n-------------------------------------------\n";
			std::ranges::for_each(
				drawing_basket,
				[&](const auto& e)
				{
					ofs << names[e.first] << L':' << e.second << L'\n';
				}
			);

			// maximum frequency finding
			std::ranges::for_each(
				drawing_basket,
				[&ordered_ranks, &names](const auto& e)
				{
					ordered_ranks.emplace(e.second, names[e.first]);
				}
			);

			// temporary display
			ofs
				<< L"\n-------------------------------------------\n"
				<< L"\tordered ranks:"
				<< L"\n-------------------------------------------\n";
			std::ranges::for_each(
				ordered_ranks,
				[&](const auto& e)
				{
					ofs << e.second << L':' << e.first << L'\n';
				}
			);

			// checking for the highest rank duplication 
			// (if the highest rank consists of more than
			// one person the simulation must be repeated 
			// till there is just one highest rank).
		} while (ordered_ranks.begin()->first == std::next(ordered_ranks.begin())->first &&
			[&]() { drawing_basket.clear(); ordered_ranks.clear(); return true; }());

		ofs.flush();
		ofs.close();
		return EXIT_SUCCESS;
	}
	catch (const std::exception& xxx)
	{
		if (ifs.is_open())
			ifs.close();
		if (ofs.is_open())
		{
			ofs.flush();
			ofs.close();
		}
		std::cerr << xxx.what() << std::endl;
		return EXIT_FAILURE;
	}
}
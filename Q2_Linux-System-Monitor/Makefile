CXX = g++
CXXFLAGS = -Wall -std=c++11 -Iinclude


procwatch: src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o procwatch

clean:
	rm -f procwatch

install: procwatch
	@echo "Installing procwatch and systemd service..."
	sudo cp procwatch /usr/local/bin/
	sudo cp src/config /etc/procwatch.conf
	sudo cp procwatch.service /etc/systemd/system/
	sudo systemctl daemon-reload
	@echo "Install complete! Run 'sudo systemctl start procwatch' to start."

uninstall:
	@echo "Removing procwatch..."
	sudo systemctl stop procwatch || true
	sudo systemctl disable procwatch || true
	sudo rm -f /usr/local/bin/procwatch
	sudo rm -f /etc/procwatch.conf
	sudo rm -f /etc/systemd/system/procwatch.service
	sudo systemctl daemon-reload
	@echo "Uninstall complete."
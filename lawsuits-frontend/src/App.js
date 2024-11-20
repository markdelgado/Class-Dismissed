import logo from './logo.svg';
import './App.css';
import LawsuitTable from './components/LawsuitTable'; 
import Hero from './components/Hero';

function App() {
  return (
    <div className="background-image">
      <Hero/>
     <LawsuitTable/>
    </div>
  );
}

export default App;
